/****************************************************************************
**
** Copyright (C) 2008-2012 NVIDIA Corporation.
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Quick 3D.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtQuick3DRuntimeRender/private/qssgrenderlayer_p.h>
#include <QtQuick3DRuntimeRender/private/qssgrendershadowmap_p.h>
#include <QtQuick3DRuntimeRender/private/qssgrenderresourcemanager_p.h>
#include <QtQuick3DRuntimeRender/private/qssgrendererimpllayerrenderdata_p.h>
#include <QtQuick3DRender/private/qssgrendershaderconstant_p.h>
#include <QtQuick3DRender/private/qssgrendershaderprogram_p.h>

QT_BEGIN_NAMESPACE

QSSGRenderShadowMap::QSSGRenderShadowMap(const QSSGRenderContextInterface &inContext)
    : m_context(inContext)
{}

QSSGRenderShadowMap::~QSSGRenderShadowMap()
{
    for (QSSGShadowMapEntry &entry : m_shadowMapList)
        entry.destroyRhiResources();

    m_shadowMapList.clear();
}

static inline void setupForRhiDepthCube(const QSSGRef<QSSGResourceManager> &resMgr,
                                        QSSGShadowMapEntry *entry,
                                        qint32 width, qint32 height, QRhiTexture::Format format)
{
    entry->m_rhiDepthCube = resMgr->allocateRhiTexture(width, height, format, QRhiTexture::RenderTarget | QRhiTexture::CubeMap);
    entry->m_rhiCubeCopy = resMgr->allocateRhiTexture(width, height, format, QRhiTexture::RenderTarget | QRhiTexture::CubeMap);
    entry->m_rhiDepthStencil = resMgr->allocateRhiRenderBuffer(width, height, QRhiRenderBuffer::DepthStencil);
}

static inline void setupForRhiDepth(const QSSGRef<QSSGResourceManager> &resMgr,
                                    QSSGShadowMapEntry *entry,
                                    qint32 width, qint32 height, QRhiTexture::Format format)
{
    entry->m_rhiDepthMap = resMgr->allocateRhiTexture(width, height, format, QRhiTexture::RenderTarget);
    entry->m_rhiDepthCopy = resMgr->allocateRhiTexture(width, height, format, QRhiTexture::RenderTarget);
    entry->m_rhiDepthStencil = resMgr->allocateRhiRenderBuffer(width, height, QRhiRenderBuffer::DepthStencil);
}

void QSSGRenderShadowMap::addShadowMapEntry(qint32 index,
                                            qint32 width,
                                            qint32 height,
                                            ShadowMapModes mode)
{
    const QSSGRef<QSSGResourceManager> &theManager(m_context.resourceManager());
    QRhi *rhi = m_context.renderContext()->rhiContext()->isValid()
            ? m_context.renderContext()->rhiContext()->rhi()
            : nullptr;

    const QSSGRenderTextureFormat glFormat = QSSGRenderTextureFormat::R16F;
    QRhiTexture::Format rhiFormat = QRhiTexture::R16F;
    if (rhi && !rhi->isTextureFormatSupported(rhiFormat))
        rhiFormat = QRhiTexture::R16;

    // This function is called once per shadow casting light on every layer
    // prepare (i.e. once per frame). We must avoid creating resources as much
    // as possible: if the shadow mode, dimensions, etc. are all the same as in
    // the previous prepare round, then reuse the existing resources.

    QSSGShadowMapEntry *pEntry = nullptr;
    if (index < m_shadowMapList.size())
        pEntry = &m_shadowMapList[index];

    if (pEntry) {
        if (rhi) {
            // RHI
            if (pEntry->m_rhiDepthMap && mode == ShadowMapModes::CUBE) {
                // previously VSM now CUBE
                pEntry->destroyRhiResources();
                setupForRhiDepthCube(theManager, pEntry, width, height, rhiFormat);
            } else if (pEntry->m_rhiDepthCube && mode != ShadowMapModes::CUBE) {
                // previously CUBE now VSM
                pEntry->destroyRhiResources();
                setupForRhiDepth(theManager, pEntry, width, height, rhiFormat);
            } else if (pEntry->m_rhiDepthMap) {
                // VSM before and now, see if size has changed
                if (pEntry->m_rhiDepthMap->pixelSize() != QSize(width, height)) {
                    pEntry->destroyRhiResources();
                    setupForRhiDepth(theManager, pEntry, width, height, rhiFormat);
                }
            } else {
                // CUBE before and now, see if size has changed
                if (pEntry->m_rhiDepthCube->pixelSize() != QSize(width, height)) {
                    pEntry->destroyRhiResources();
                    setupForRhiDepthCube(theManager, pEntry, width, height, rhiFormat);
                }
            }
        } else {
            // legacy GL
            if ((nullptr != pEntry->m_depthMap) && (mode == ShadowMapModes::CUBE)) {
                theManager->release(pEntry->m_depthMap);
                theManager->release(pEntry->m_depthCopy);
                theManager->release(pEntry->m_depthRender);
                pEntry->m_depthCube = theManager->allocateTextureCube(width, height, glFormat, 1);
                pEntry->m_cubeCopy = theManager->allocateTextureCube(width, height, glFormat, 1);
                pEntry->m_depthRender = theManager->allocateTexture2D(width, height, QSSGRenderTextureFormat::Depth24Stencil8, 1);
                pEntry->m_depthMap = nullptr;
                pEntry->m_depthCopy = nullptr;
            } else if ((nullptr != pEntry->m_depthCube) && (mode != ShadowMapModes::CUBE)) {
                theManager->release(pEntry->m_depthCube);
                theManager->release(pEntry->m_cubeCopy);
                theManager->release(pEntry->m_depthRender);
                pEntry->m_depthMap = theManager->allocateTexture2D(width, height, glFormat, 1);
                pEntry->m_depthCopy = theManager->allocateTexture2D(width, height, glFormat, 1);
                pEntry->m_depthCube = nullptr;
                pEntry->m_cubeCopy = nullptr;
                pEntry->m_depthRender = theManager->allocateTexture2D(width, height, QSSGRenderTextureFormat::Depth24Stencil8, 1);
            } else if (nullptr != pEntry->m_depthMap) {
                QSSGTextureDetails theDetails(pEntry->m_depthMap->textureDetails());

                // If anything differs about the map we're looking for, let's recreate it.
                if (theDetails.format != glFormat || theDetails.width != width || theDetails.height != height) {
                    // release texture
                    theManager->release(pEntry->m_depthMap);
                    theManager->release(pEntry->m_depthCopy);
                    theManager->release(pEntry->m_depthRender);
                    pEntry->m_depthMap = theManager->allocateTexture2D(width, height, glFormat, 1);
                    pEntry->m_depthCopy = theManager->allocateTexture2D(width, height, glFormat, 1);
                    pEntry->m_depthCube = nullptr;
                    pEntry->m_cubeCopy = nullptr;
                    pEntry->m_depthRender = theManager->allocateTexture2D(width, height, QSSGRenderTextureFormat::Depth24Stencil8, 1);
                }
            } else {
                QSSGTextureDetails theDetails(pEntry->m_depthCube->textureDetails());

                // If anything differs about the map we're looking for, let's recreate it.
                if (theDetails.format != glFormat || theDetails.width != width || theDetails.height != height) {
                    // release texture
                    theManager->release(pEntry->m_depthCube);
                    theManager->release(pEntry->m_cubeCopy);
                    theManager->release(pEntry->m_depthRender);
                    pEntry->m_depthCube = theManager->allocateTextureCube(width, height, glFormat, 1);
                    pEntry->m_cubeCopy = theManager->allocateTextureCube(width, height, glFormat, 1);
                    pEntry->m_depthRender = theManager->allocateTexture2D(width, height, QSSGRenderTextureFormat::Depth24Stencil8, 1);
                    pEntry->m_depthMap = nullptr;
                    pEntry->m_depthCopy = nullptr;
                }
            }
        }

        pEntry->m_shadowMapMode = mode;
    } else if (mode == ShadowMapModes::CUBE) {
        if (rhi) {
            // RHI
            QRhiTexture *depthMap = theManager->allocateRhiTexture(width, height, rhiFormat, QRhiTexture::RenderTarget | QRhiTexture::CubeMap);
            QRhiTexture *depthCopy = theManager->allocateRhiTexture(width, height, rhiFormat, QRhiTexture::RenderTarget | QRhiTexture::CubeMap);
            QRhiRenderBuffer *depthStencil = theManager->allocateRhiRenderBuffer(width, height, QRhiRenderBuffer::DepthStencil);
            m_shadowMapList.push_back(QSSGShadowMapEntry::withRhiDepthCubeMap(index, mode, depthMap, depthCopy, depthStencil));
        } else {
            // legacy GL
            QSSGRef<QSSGRenderTextureCube> theDepthTex = theManager->allocateTextureCube(width, height, glFormat, 1);
            QSSGRef<QSSGRenderTextureCube> theDepthCopy = theManager->allocateTextureCube(width, height, glFormat, 1);
            QSSGRef<QSSGRenderTexture2D> theDepthTemp = theManager->allocateTexture2D(width,
                                                                                      height,
                                                                                      QSSGRenderTextureFormat::Depth24Stencil8,
                                                                                      1);
            m_shadowMapList.push_back(QSSGShadowMapEntry::withGlDepthCubeMap(index, mode, theDepthTex, theDepthCopy, theDepthTemp));
        }

        pEntry = &m_shadowMapList.back();
    } else { // VSM
        Q_ASSERT(mode == ShadowMapModes::VSM);
        if (rhi) {
            // RHI
            QRhiTexture *depthMap = theManager->allocateRhiTexture(width, height, rhiFormat, QRhiTexture::RenderTarget);
            QRhiTexture *depthCopy = theManager->allocateRhiTexture(width, height, rhiFormat, QRhiTexture::RenderTarget);
            QRhiRenderBuffer *depthStencil = theManager->allocateRhiRenderBuffer(width, height, QRhiRenderBuffer::DepthStencil);
            m_shadowMapList.push_back(QSSGShadowMapEntry::withRhiDepthMap(index, mode, depthMap, depthCopy, depthStencil));
        } else {
            // legacy GL
            QSSGRef<QSSGRenderTexture2D> theDepthMap = theManager->allocateTexture2D(width, height, glFormat, 1);
            QSSGRef<QSSGRenderTexture2D> theDepthCopy = theManager->allocateTexture2D(width, height, glFormat, 1);
            QSSGRef<QSSGRenderTexture2D> theDepthTemp = theManager->allocateTexture2D(width,
                                                                                      height,
                                                                                      QSSGRenderTextureFormat::Depth24Stencil8,
                                                                                      1);
            m_shadowMapList.push_back(QSSGShadowMapEntry::withGlDepthMap(index, mode, theDepthMap, theDepthCopy, theDepthTemp));
        }

        pEntry = &m_shadowMapList.back();
    }

    if (pEntry) {
        // Additional graphics resources: samplers, render targets.

        if (rhi) {
            if (mode == ShadowMapModes::VSM) {
                if (pEntry->m_rhiRenderTargets.isEmpty()) {
                    pEntry->m_rhiRenderTargets.resize(1);
                    pEntry->m_rhiRenderTargets[0] = nullptr;
                }
                Q_ASSERT(pEntry->m_rhiRenderTargets.count() == 1);

                QRhiTextureRenderTarget *&rt(pEntry->m_rhiRenderTargets[0]);
                if (!rt) {
                    QRhiTextureRenderTargetDescription rtDesc;
                    rtDesc.setColorAttachments({ pEntry->m_rhiDepthMap });
                    rtDesc.setDepthStencilBuffer(pEntry->m_rhiDepthStencil);
                    rt = rhi->newTextureRenderTarget(rtDesc);
                    rt->setDescription(rtDesc);
                    // The same renderpass descriptor can be reused since the
                    // format, load/store ops are the same regardless of the shadow mode.
                    if (!pEntry->m_rhiRenderPassDesc)
                        pEntry->m_rhiRenderPassDesc = rt->newCompatibleRenderPassDescriptor();
                    rt->setRenderPassDescriptor(pEntry->m_rhiRenderPassDesc);
                    if (!rt->build())
                        qWarning("Failed to build shadow map render target");
                }

                if (!pEntry->m_rhiBlurRenderTarget0) {
                    // blur X: depthMap -> depthCopy
                    pEntry->m_rhiBlurRenderTarget0 = rhi->newTextureRenderTarget({ pEntry->m_rhiDepthCopy });
                    if (!pEntry->m_rhiBlurRenderPassDesc)
                        pEntry->m_rhiBlurRenderPassDesc = pEntry->m_rhiBlurRenderTarget0->newCompatibleRenderPassDescriptor();
                    pEntry->m_rhiBlurRenderTarget0->setRenderPassDescriptor(pEntry->m_rhiBlurRenderPassDesc);
                    pEntry->m_rhiBlurRenderTarget0->build();
                }
                if (!pEntry->m_rhiBlurRenderTarget1) {
                    // blur Y: depthCopy -> depthMap
                    pEntry->m_rhiBlurRenderTarget1 = rhi->newTextureRenderTarget({ pEntry->m_rhiDepthMap });
                    pEntry->m_rhiBlurRenderTarget1->setRenderPassDescriptor(pEntry->m_rhiBlurRenderPassDesc);
                    pEntry->m_rhiBlurRenderTarget1->build();
                }
            } else {
                if (pEntry->m_rhiRenderTargets.isEmpty()) {
                    pEntry->m_rhiRenderTargets.resize(6);
                    for (int i = 0; i < 6; ++i)
                        pEntry->m_rhiRenderTargets[i] = nullptr;
                }
                Q_ASSERT(pEntry->m_rhiRenderTargets.count() == 6);

                for (int face = 0; face < 6; ++face) {
                    QRhiTextureRenderTarget *&rt(pEntry->m_rhiRenderTargets[face]);
                    if (!rt) {
                        QRhiColorAttachment att(pEntry->m_rhiDepthCube);
                        att.setLayer(face); // 6 render targets, each referencing one face of the cubemap
                        QRhiTextureRenderTargetDescription rtDesc;
                        rtDesc.setColorAttachments({ att });
                        rtDesc.setDepthStencilBuffer(pEntry->m_rhiDepthStencil);
                        rt = rhi->newTextureRenderTarget(rtDesc);
                        rt->setDescription(rtDesc);
                        if (!pEntry->m_rhiRenderPassDesc)
                            pEntry->m_rhiRenderPassDesc = rt->newCompatibleRenderPassDescriptor();
                        rt->setRenderPassDescriptor(pEntry->m_rhiRenderPassDesc);
                        if (!rt->build())
                            qWarning("Failed to build shadow map render target");
                    }
                }

                // blurring cubemap happens via multiple render targets (all faces attached to COLOR0..5)
                if (rhi->resourceLimit(QRhi::MaxColorAttachments) >= 6) {
                    if (!pEntry->m_rhiBlurRenderTarget0) {
                        // blur X: depthCube -> cubeCopy
                        QRhiColorAttachment att[6];
                        for (int face = 0; face < 6; ++face) {
                            att[face].setTexture(pEntry->m_rhiCubeCopy);
                            att[face].setLayer(face);
                        }
                        QRhiTextureRenderTargetDescription rtDesc;
                        rtDesc.setColorAttachments(att, att + 6);
                        pEntry->m_rhiBlurRenderTarget0 = rhi->newTextureRenderTarget(rtDesc);
                        if (!pEntry->m_rhiBlurRenderPassDesc)
                            pEntry->m_rhiBlurRenderPassDesc = pEntry->m_rhiBlurRenderTarget0->newCompatibleRenderPassDescriptor();
                        pEntry->m_rhiBlurRenderTarget0->setRenderPassDescriptor(pEntry->m_rhiBlurRenderPassDesc);
                        pEntry->m_rhiBlurRenderTarget0->build();
                    }
                    if (!pEntry->m_rhiBlurRenderTarget1) {
                        // blur Y: cubeCopy -> depthCube
                        QRhiColorAttachment att[6];
                        for (int face = 0; face < 6; ++face) {
                            att[face].setTexture(pEntry->m_rhiDepthCube);
                            att[face].setLayer(face);
                        }
                        QRhiTextureRenderTargetDescription rtDesc;
                        rtDesc.setColorAttachments(att, att + 6);
                        pEntry->m_rhiBlurRenderTarget1 = rhi->newTextureRenderTarget(rtDesc);
                        pEntry->m_rhiBlurRenderTarget1->setRenderPassDescriptor(pEntry->m_rhiBlurRenderPassDesc);
                        pEntry->m_rhiBlurRenderTarget1->build();
                    }
                } else {
                    static bool warned = false;
                    if (!warned) {
                        warned = true;
                        qWarning("Cubemap-based shadow maps will not be blurred because MaxColorAttachments is less than 6");
                    }
                }
            }

        } else {
            // legacy GL
            if (pEntry->m_depthMap) {
                pEntry->m_depthMap->setMinFilter(QSSGRenderTextureMinifyingOp::Linear);
                pEntry->m_depthMap->setMagFilter(QSSGRenderTextureMagnifyingOp::Linear);
                pEntry->m_depthMap->setTextureWrapS(QSSGRenderTextureCoordOp::ClampToEdge);
                pEntry->m_depthMap->setTextureWrapT(QSSGRenderTextureCoordOp::ClampToEdge);

                pEntry->m_depthCopy->setMinFilter(QSSGRenderTextureMinifyingOp::Linear);
                pEntry->m_depthCopy->setMagFilter(QSSGRenderTextureMagnifyingOp::Linear);
                pEntry->m_depthCopy->setTextureWrapS(QSSGRenderTextureCoordOp::ClampToEdge);
                pEntry->m_depthCopy->setTextureWrapT(QSSGRenderTextureCoordOp::ClampToEdge);

                pEntry->m_depthRender->setMinFilter(QSSGRenderTextureMinifyingOp::Linear);
                pEntry->m_depthRender->setMagFilter(QSSGRenderTextureMagnifyingOp::Linear);
                pEntry->m_depthRender->setTextureWrapS(QSSGRenderTextureCoordOp::ClampToEdge);
                pEntry->m_depthRender->setTextureWrapT(QSSGRenderTextureCoordOp::ClampToEdge);
            } else {
                pEntry->m_depthCube->setMinFilter(QSSGRenderTextureMinifyingOp::Linear);
                pEntry->m_depthCube->setMagFilter(QSSGRenderTextureMagnifyingOp::Linear);
                pEntry->m_depthCube->setTextureWrapS(QSSGRenderTextureCoordOp::ClampToEdge);
                pEntry->m_depthCube->setTextureWrapT(QSSGRenderTextureCoordOp::ClampToEdge);

                pEntry->m_cubeCopy->setMinFilter(QSSGRenderTextureMinifyingOp::Linear);
                pEntry->m_cubeCopy->setMagFilter(QSSGRenderTextureMagnifyingOp::Linear);
                pEntry->m_cubeCopy->setTextureWrapS(QSSGRenderTextureCoordOp::ClampToEdge);
                pEntry->m_cubeCopy->setTextureWrapT(QSSGRenderTextureCoordOp::ClampToEdge);

                pEntry->m_depthRender->setMinFilter(QSSGRenderTextureMinifyingOp::Linear);
                pEntry->m_depthRender->setMagFilter(QSSGRenderTextureMagnifyingOp::Linear);
                pEntry->m_depthRender->setTextureWrapS(QSSGRenderTextureCoordOp::ClampToEdge);
                pEntry->m_depthRender->setTextureWrapT(QSSGRenderTextureCoordOp::ClampToEdge);
            }
        }

        pEntry->m_lightIndex = index;
    }
}

QSSGShadowMapEntry *QSSGRenderShadowMap::getShadowMapEntry(int index)
{
    if (index < 0) {
        Q_UNREACHABLE();
        return nullptr;
    }
    QSSGShadowMapEntry *pEntry = nullptr;

    for (int i = 0; i < m_shadowMapList.size(); i++) {
        pEntry = &m_shadowMapList[i];
        if (pEntry->m_lightIndex == quint32(index))
            return pEntry;
    }

    return nullptr;
}

QSSGRef<QSSGRenderShadowMap> QSSGRenderShadowMap::create(const QSSGRenderContextInterface &inContext)
{
    return QSSGRef<QSSGRenderShadowMap>(new QSSGRenderShadowMap(inContext));
}

QT_END_NAMESPACE
