/****************************************************************************
**
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

#ifndef QSSGPRINCIPLEDMATERIAL_H
#define QSSGPRINCIPLEDMATERIAL_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtQuick3D/private/qquick3dmaterial_p.h>
#include <QtQuick3D/private/qquick3dtexture_p.h>

#include <QColor>
#include <QHash>

QT_BEGIN_NAMESPACE

class Q_QUICK3D_EXPORT QQuick3DPrincipledMaterial : public QQuick3DMaterial
{
    Q_OBJECT
    Q_PROPERTY(QSSGPrincipledMaterialLighting lighting READ lighting WRITE setLighting NOTIFY lightingChanged)
    Q_PROPERTY(QSSGPrincipledMaterialBlendMode blendMode READ blendMode WRITE setBlendMode NOTIFY blendModeChanged)

    Q_PROPERTY(QColor baseColor READ baseColor WRITE setBaseColor NOTIFY baseColorChanged)
    Q_PROPERTY(QQuick3DTexture *baseColorMap READ baseColorMap WRITE setBaseColorMap NOTIFY baseColorMapChanged)

    Q_PROPERTY(float metalness READ metalness WRITE setMetalness NOTIFY metalnessChanged)
    Q_PROPERTY(QQuick3DTexture *metalnessMap READ metalnessMap WRITE setMetalnessMap NOTIFY metalnessMapChanged)

    Q_PROPERTY(float specularAmount READ specularAmount WRITE setSpecularAmount NOTIFY specularAmountChanged)
    Q_PROPERTY(QQuick3DTexture *specularMap READ specularMap WRITE setSpecularMap NOTIFY specularMapChanged)
    Q_PROPERTY(float specularTint READ specularTint WRITE setSpecularTint NOTIFY specularTintChanged)

    Q_PROPERTY(float roughness READ roughness WRITE setRoughness NOTIFY roughnessChanged)
    Q_PROPERTY(QQuick3DTexture *roughnessMap READ roughnessMap WRITE setRoughnessMap NOTIFY roughnessMapChanged)

    Q_PROPERTY(float indexOfRefraction READ indexOfRefraction WRITE setIndexOfRefraction NOTIFY indexOfRefractionChanged)

    Q_PROPERTY(QColor emissiveColor READ emissiveColor WRITE setEmissiveColor NOTIFY emissiveColorChanged)
    Q_PROPERTY(QQuick3DTexture *emissiveMap READ emissiveMap WRITE setEmissiveMap NOTIFY emissiveMapChanged)

    Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
    Q_PROPERTY(QQuick3DTexture *opacityMap READ opacityMap WRITE setOpacityMap NOTIFY opacityMapChanged)

    Q_PROPERTY(QQuick3DTexture *normalMap READ normalMap WRITE setNormalMap NOTIFY normalMapChanged)
    Q_PROPERTY(float normalStrength READ normalStrength WRITE setNormalStrength NOTIFY normalStrengthChanged)

    Q_PROPERTY(QQuick3DTexture *specularReflectionMap READ specularReflectionMap WRITE setSpecularReflectionMap NOTIFY specularReflectionMapChanged)

public:
    enum QSSGPrincipledMaterialLighting { NoLighting = 0, VertexLighting, FragmentLighting };
    Q_ENUM(QSSGPrincipledMaterialLighting)

    enum QSSGPrincipledMaterialBlendMode { Normal = 0, Screen, Multiply, Overlay, ColorBurn, ColorDodge };
    Q_ENUM(QSSGPrincipledMaterialBlendMode)



    QQuick3DPrincipledMaterial();
    ~QQuick3DPrincipledMaterial() override;

    QQuick3DObject::Type type() const override;

    QSSGPrincipledMaterialLighting lighting() const;
    QSSGPrincipledMaterialBlendMode blendMode() const;
    QColor baseColor() const;
    QQuick3DTexture *baseColorMap() const;
    float emissivePower() const;
    QQuick3DTexture *emissiveMap() const;
    QColor emissiveColor() const;
    QQuick3DTexture *specularReflectionMap() const;
    QQuick3DTexture *specularMap() const;
    float specularTint() const;
    float indexOfRefraction() const;
    float specularAmount() const;
    float roughness() const;
    QQuick3DTexture *roughnessMap() const;
    float opacity() const;
    QQuick3DTexture *opacityMap() const;
    QQuick3DTexture *normalMap() const;
    float metalness() const;
    QQuick3DTexture *metalnessMap() const;
    float normalStrength() const;

public Q_SLOTS:
    void setLighting(QSSGPrincipledMaterialLighting lighting);
    void setBlendMode(QSSGPrincipledMaterialBlendMode blendMode);
    void setBaseColor(QColor baseColor);
    void setBaseColorMap(QQuick3DTexture *baseColorMap);
    void setEmissiveMap(QQuick3DTexture *emissiveMap);

    void setEmissiveColor(QColor emissiveColor);
    void setSpecularReflectionMap(QQuick3DTexture *specularReflectionMap);
    void setSpecularMap(QQuick3DTexture *specularMap);
    void setSpecularTint(float specularTint);
    void setIndexOfRefraction(float indexOfRefraction);
    void setSpecularAmount(float specularAmount);
    void setRoughness(float roughness);
    void setRoughnessMap(QQuick3DTexture *roughnessMap);
    void setOpacity(float opacity);
    void setOpacityMap(QQuick3DTexture *opacityMap);
    void setNormalMap(QQuick3DTexture *normalMap);
    void setMetalness(float metalnessAmount);
    void setMetalnessMap(QQuick3DTexture * metalnessMap);
    void setNormalStrength(float normalStrength);

Q_SIGNALS:
    void lightingChanged(QSSGPrincipledMaterialLighting lighting);
    void blendModeChanged(QSSGPrincipledMaterialBlendMode blendMode);
    void baseColorChanged(QColor baseColor);
    void baseColorMapChanged(QQuick3DTexture *baseColorMap);
    void emissiveMapChanged(QQuick3DTexture *emissiveMap);
    void emissiveColorChanged(QColor emissiveColor);
    void specularReflectionMapChanged(QQuick3DTexture *specularReflectionMap);
    void specularMapChanged(QQuick3DTexture *specularMap);
    void specularTintChanged(float specularTint);
    void indexOfRefractionChanged(float indexOfRefraction);
    void specularAmountChanged(float specularAmount);
    void roughnessChanged(float roughness);
    void roughnessMapChanged(QQuick3DTexture *roughnessMap);
    void opacityChanged(float opacity);
    void opacityMapChanged(QQuick3DTexture *opacityMap);
    void normalMapChanged(QQuick3DTexture *normalMap);
    void metalnessChanged(float metalness);
    void metalnessMapChanged(QQuick3DTexture * metalnessMap);
    void normalStrengthChanged(float normalStrength);

protected:
    QSSGRenderGraphObject *updateSpatialNode(QSSGRenderGraphObject *node) override;
    void itemChange(ItemChange, const ItemChangeData &) override;
private:
    using ConnectionMap = QHash<QObject*, QMetaObject::Connection>;

    enum QSSGPrincipledMaterialDirtyType {
        LightingModeDirty = 0x00000001,
        BlendModeDirty = 0x00000002,
        BaseColorDirty = 0x00000004,
        EmissiveDirty = 0x00000008,
        SpecularDirty = 0x00000010,
        OpacityDirty = 0x00000020,
        NormalDirty = 0x00000040,
        MetalnessDirty = 0x00000080,
        RoughnessDirty = 0x00000100
    };

    void updateSceneRenderer(QQuick3DSceneManager *window);
    QSSGPrincipledMaterialLighting m_lighting = VertexLighting;
    QSSGPrincipledMaterialBlendMode m_blendMode = Normal;
    QColor m_baseColor = Qt::white;
    QQuick3DTexture *m_baseColorMap = nullptr;
    QQuick3DTexture *m_emissiveMap = nullptr;
    ConnectionMap m_connections;

    QColor m_emissiveColor = Qt::black;
    QQuick3DTexture *m_specularReflectionMap = nullptr;
    QQuick3DTexture *m_specularMap = nullptr;
    QQuick3DTexture *m_roughnessMap = nullptr;
    QQuick3DTexture *m_opacityMap = nullptr;
    QQuick3DTexture *m_normalMap = nullptr;
    QQuick3DTexture *m_metalnessMap = nullptr;
    float m_emissivePower = 0.0f;
    float m_specularTint = 0.0f;
    float m_indexOfRefraction = 1.45f;
    float m_specularAmount = 0.0f;
    float m_roughness = 0.0f;
    float m_opacity = 1.0f;
    float m_metalnessAmount = 1.0f;
    float m_normalStrength = 1.0f;

    quint32 m_dirtyAttributes = 0xffffffff; // all dirty by default
    void markDirty(QSSGPrincipledMaterialDirtyType type);
    static void updateProperyListener(QQuick3DObject *,
                                      QQuick3DObject *,
                                      QQuick3DSceneManager *,
                                      QQuick3DPrincipledMaterial::ConnectionMap &,
                                      std::function<void(QQuick3DObject *o)>);
};

QT_END_NAMESPACE

#endif // QSSGPRINCIPLEDMATERIAL_H