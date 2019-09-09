/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tests of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick3D 1.0
import QtQuick 2.12
import QtQuick.Timeline 1.0
import QtQuick3D.MaterialLibrary 1.0
import "./materials" as Materials

Rectangle {
    id: imagebasedlighting
    width: 1920
    height: 1080
    color: Qt.rgba(0, 0, 0, 1)

    View3D {
        id: layer
        anchors.left: parent.left
        anchors.leftMargin: parent.width * 0
        width: parent.width * 0.5
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0
        height: parent.height * 1
        environment: SceneEnvironment {
            clearColor: Qt.rgba(0, 0, 0, 1)
            aoDither: true
            isDepthPrePassDisabled: false
            lightProbe: layer_lightprobe
            probeBrightness: 1000
            lightProbe2: layer_lightprobe2
            probe2Fade: 0.33000001311302185
        }

        Camera {
            id: camera
            position: Qt.vector3d(0, 0, -1000)
            rotationOrder: Node.YZX
            clipFar: 5000
            isFieldOfViewHorizontal: true
        }

        Texture {
            id: layer_lightprobe
            source: "maps/OpenfootageNET_fieldairport-512.hdr"
            mappingMode: Texture.LightProbe
            tilingModeHorizontal: Texture.Repeat
        }

        Texture {
            id: layer_lightprobe2
            source: "maps/OpenfootageNET_Gerlos-512.hdr"
            mappingMode: Texture.LightProbe
            tilingModeHorizontal: Texture.Repeat
        }

        Node {
            id: group
            position: Qt.vector3d(468.305, -129.677, 0)
            scale: Qt.vector3d(2, 2, 2)
            rotationOrder: Node.YZX

            Model {
                id: cylinder
                position: Qt.vector3d(-448.399, 78.869, 0)
                rotationOrder: Node.YZX
                source: "#Cylinder"
                edgeTess: 4
                innerTess: 4

                Materials.Default_002 {
                    id: default_
                }
                materials: [default_]
            }

            Model {
                id: cone
                position: Qt.vector3d(-452.899, -147.545, 0)
                rotationOrder: Node.YZX
                source: "#Cone"
                edgeTess: 4
                innerTess: 4

                CopperMaterial {
                    id: default_001
                }
                materials: [default_001]
            }

            Model {
                id: sphere
                position: Qt.vector3d(-441.352, 255.317, 0)
                rotationOrder: Node.YZX
                source: "#Sphere"
                edgeTess: 4
                innerTess: 4

                DefaultMaterial {
                    id: default_002
                    lighting: DefaultMaterial.FragmentLighting
                    indexOfRefraction: 1.5
                    specularAmount: 0
                    specularRoughness: 0
                    bumpAmount: 0.5
                    translucentFalloff: 1
                    displacementAmount: 20
                }
                materials: [default_002]
            }

            Model {
                id: defaultOverride
                position: Qt.vector3d(-233.489, 252.353, 0)
                rotationOrder: Node.YZX
                source: "#Sphere"
                edgeTess: 4
                innerTess: 4

                DefaultMaterial {
                    id: default_003
                    lighting: DefaultMaterial.FragmentLighting
                    indexOfRefraction: 1.5
                    specularAmount: 0
                    specularRoughness: 0
                    bumpAmount: 0.5
                    translucentFalloff: 1
                    iblProbe: default_003_iblprobe
                    displacementAmount: 20

                    Texture {
                        id: default_003_iblprobe
                        source: "maps/OpenfootageNET_lowerAustria01-512.hdr"
                        mappingMode: Texture.LightProbe
                        tilingModeHorizontal: Texture.Repeat
                    }
                }
                materials: [default_003]
            }

            Model {
                id: referenceMaterialOverride
                position: Qt.vector3d(-235.717, 78.869, 0)
                rotationOrder: Node.YZX
                source: "#Cylinder"
                edgeTess: 4
                innerTess: 4

                Materials.Default_002 {
                    id: default_004
                    iblProbe: default_004_iblprobe

                    Texture {
                        id: default_004_iblprobe
                        source: "maps/OpenfootageNET_lowerAustria01-512.hdr"
                        mappingMode: Texture.LightProbe
                        tilingModeHorizontal: Texture.Repeat
                    }
                }
                materials: [default_004]
            }

            Model {
                id: customMaterialOverride
                position: Qt.vector3d(-245.686, -147.545, 0)
                rotationOrder: Node.YZX
                source: "#Cone"
                edgeTess: 4
                innerTess: 4

                CopperMaterial {
                    id: default_005
                    iblProbe: default_005_iblprobe

                    Texture {
                        id: default_005_iblprobe
                        source: "maps/OpenfootageNET_lowerAustria01-512.hdr"
                        mappingMode: Texture.LightProbe
                        tilingModeHorizontal: Texture.Repeat
                    }
                }
                materials: [default_005]
            }

            Model {
                id: defaultMaterialSpecial
                position: Qt.vector3d(-18.5029, 255.317, 0)
                rotationOrder: Node.YZX
                source: "#Sphere"
                edgeTess: 4
                innerTess: 4

                DefaultMaterial {
                    id: default_006
                    lighting: DefaultMaterial.FragmentLighting
                    indexOfRefraction: 1.5
                    specularAmount: 0
                    specularRoughness: 0
                    bumpAmount: 0.5
                    translucentFalloff: 1
                    iblProbe: default_006_iblprobe
                    displacementAmount: 20

                    Texture {
                        id: default_006_iblprobe
                        source: "maps/OpenfootageNET_lowerAustria01-512.hdr"
                        scaleU: 20
                        scaleV: 0
                        mappingMode: Texture.LightProbe
                        tilingModeHorizontal: Texture.Repeat
                    }
                }
                materials: [default_006]
            }

            Model {
                id: referenceMaterialSpecial
                position: Qt.vector3d(-14.5576, 78.869, 0)
                rotationOrder: Node.YZX
                source: "#Cylinder"
                edgeTess: 4
                innerTess: 4

                Materials.Default_002 {
                    id: default_007
                    iblProbe: default_007_iblprobe

                    Texture {
                        id: default_007_iblprobe
                        source: "maps/OpenfootageNET_lowerAustria01-512.hdr"
                        scaleU: 20
                        mappingMode: Texture.LightProbe
                        tilingModeHorizontal: Texture.Repeat
                    }
                }
                materials: [default_007]
            }

            Model {
                id: customMaterialSpecial
                position: Qt.vector3d(-17.4744, -147.545, 0)
                rotationOrder: Node.YZX
                source: "#Cone"
                edgeTess: 4
                innerTess: 4

                CopperMaterial {
                    id: copper_001
                    iblProbe: copper_001_iblprobe

                    Texture {
                        id: copper_001_iblprobe
                        source: "maps/OpenfootageNET_lowerAustria01-512.hdr"
                        scaleU: 20
                        mappingMode: Texture.LightProbe
                        tilingModeHorizontal: Texture.Repeat
                    }
                }
                materials: [copper_001]
            }
        }
    }

    View3D {
        id: oneLightProbe
        anchors.left: parent.left
        anchors.leftMargin: parent.width * 0.5
        width: parent.width * 0.5
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0
        height: parent.height * 1
        environment: SceneEnvironment {
            clearColor: Qt.rgba(0, 0, 0, 1)
            aoDither: true
            isDepthPrePassDisabled: false
            lightProbe: oneLightProbe_lightprobe
            probeBrightness: 1000
        }

        Camera {
            id: camera_001
            position: Qt.vector3d(0, 0, -1000)
            rotationOrder: Node.YZX
            clipFar: 5000
            isFieldOfViewHorizontal: true
        }

        Texture {
            id: oneLightProbe_lightprobe
            source: "maps/OpenfootageNET_fieldairport-512.hdr"
            mappingMode: Texture.LightProbe
            tilingModeHorizontal: Texture.Repeat
        }

        Node {
            id: group_001
            position: Qt.vector3d(468.305, -129.677, 0)
            scale: Qt.vector3d(2, 2, 2)
            rotationOrder: Node.YZX

            Model {
                id: referenceMaterial
                position: Qt.vector3d(-448.399, 78.869, 0)
                rotationOrder: Node.YZX
                source: "#Cylinder"
                edgeTess: 4
                innerTess: 4

                Materials.Default_009 {
                    id: default_008
                }
                materials: [default_008]
            }

            Model {
                id: customMaterial
                position: Qt.vector3d(-452.899, -147.545, 0)
                rotationOrder: Node.YZX
                source: "#Cone"
                edgeTess: 4
                innerTess: 4

                CopperMaterial {
                    id: copper_002
                }
                materials: [copper_002]
            }

            Model {
                id: defaultMaterial
                position: Qt.vector3d(-441.352, 255.317, 0)
                rotationOrder: Node.YZX
                source: "#Sphere"
                edgeTess: 4
                innerTess: 4

                DefaultMaterial {
                    id: default_009
                    lighting: DefaultMaterial.FragmentLighting
                    indexOfRefraction: 1.5
                    specularAmount: 0
                    specularRoughness: 0
                    bumpAmount: 0.5
                    translucentFalloff: 1
                    displacementAmount: 20
                }
                materials: [default_009]
            }

            Model {
                id: defaultOverride_001
                position: Qt.vector3d(-233.489, 252.353, 0)
                rotationOrder: Node.YZX
                source: "#Sphere"
                edgeTess: 4
                innerTess: 4

                DefaultMaterial {
                    id: default_010
                    lighting: DefaultMaterial.FragmentLighting
                    indexOfRefraction: 1.5
                    specularAmount: 0
                    specularRoughness: 0
                    bumpAmount: 0.5
                    translucentFalloff: 1
                    iblProbe: default_010_iblprobe
                    displacementAmount: 20

                    Texture {
                        id: default_010_iblprobe
                        source: "maps/OpenfootageNET_lowerAustria01-512.hdr"
                        mappingMode: Texture.LightProbe
                        tilingModeHorizontal: Texture.Repeat
                    }
                }
                materials: [default_010]
            }

            Model {
                id: referenceMaterialOverride_001
                position: Qt.vector3d(-235.717, 78.869, 0)
                rotationOrder: Node.YZX
                source: "#Cylinder"
                edgeTess: 4
                innerTess: 4

                Materials.Default_009 {
                    id: default_011
                    iblProbe: default_011_iblprobe

                    Texture {
                        id: default_011_iblprobe
                        source: "maps/OpenfootageNET_lowerAustria01-512.hdr"
                        mappingMode: Texture.LightProbe
                        tilingModeHorizontal: Texture.Repeat
                    }
                }
                materials: [default_011]
            }

            Model {
                id: customMaterialOverride_001
                position: Qt.vector3d(-245.686, -147.545, 0)
                rotationOrder: Node.YZX
                source: "#Cone"
                edgeTess: 4
                innerTess: 4

                CopperMaterial {
                    id: copper_003
                    iblProbe: copper_003_iblprobe

                    Texture {
                        id: copper_003_iblprobe
                        source: "maps/OpenfootageNET_lowerAustria01-512.hdr"
                        mappingMode: Texture.LightProbe
                        tilingModeHorizontal: Texture.Repeat
                    }
                }
                materials: [copper_003]
            }

            Model {
                id: defaultMaterialSpecial_001
                position: Qt.vector3d(-18.5029, 255.317, 0)
                rotationOrder: Node.YZX
                source: "#Sphere"
                edgeTess: 4
                innerTess: 4

                DefaultMaterial {
                    id: default_012
                    lighting: DefaultMaterial.FragmentLighting
                    indexOfRefraction: 1.5
                    specularAmount: 0
                    specularRoughness: 0
                    bumpAmount: 0.5
                    translucentFalloff: 1
                    iblProbe: default_012_iblprobe
                    displacementAmount: 20

                    Texture {
                        id: default_012_iblprobe
                        source: "maps/OpenfootageNET_lowerAustria01-512.hdr"
                        scaleU: 20
                        scaleV: 0
                        mappingMode: Texture.LightProbe
                        tilingModeHorizontal: Texture.Repeat
                    }
                }
                materials: [default_012]
            }

            Model {
                id: referenceMaterialSpecial_001
                position: Qt.vector3d(-14.5576, 78.869, 0)
                rotationOrder: Node.YZX
                source: "#Cylinder"
                edgeTess: 4
                innerTess: 4

                Materials.Default_009 {
                    id: default_013
                    iblProbe: default_013_iblprobe

                    Texture {
                        id: default_013_iblprobe
                        source: "maps/OpenfootageNET_lowerAustria01-512.hdr"
                        scaleU: 20
                        mappingMode: Texture.LightProbe
                        tilingModeHorizontal: Texture.Repeat
                    }
                }
                materials: [default_013]
            }

            Model {
                id: customMaterialSpecial_001
                position: Qt.vector3d(-17.4744, -147.545, 0)
                rotationOrder: Node.YZX
                source: "#Cone"
                edgeTess: 4
                innerTess: 4

                CopperMaterial {
                    id: copper_004
                    iblProbe: copper_004_iblprobe

                    Texture {
                        id: copper_004_iblprobe
                        source: "maps/OpenfootageNET_lowerAustria01-512.hdr"
                        scaleU: 20
                        mappingMode: Texture.LightProbe
                        tilingModeHorizontal: Texture.Repeat
                    }
                }
                materials: [copper_004]
            }
        }
    }

    Timeline {
        id: slide1Timeline
        startFrame: 0
        endFrame: 10
        currentFrame: 0
        enabled: false
        animations: [
            TimelineAnimation {
                id: slide1TimelineAnimation
                duration: 10000
                from: 0
                to: 10
                running: true
                loops: 1
                pingPong: false
            }
        ]
    }

    states: [
        State {
            name: "Slide1"
            PropertyChanges {
                target: slide1Timeline
                enabled: true
                currentFrame: 0
            }
            PropertyChanges {
                target: slide1TimelineAnimation
                running: true
            }
        }
    ]
    state: "Slide1"
}