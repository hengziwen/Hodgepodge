# 配置逐节索引

[知识库首页](../README.md) · [参考入口](README.md)

> 自动生成的静态导航；行为结论以人工章节和源码为准。

这是项目默认配置摘录，带行号；凭据类字段值已隐去。不包含用户 Saved/Config、引擎配置或地图蓝图覆盖。

## DefaultEditor.ini

[打开源配置](../../../Config/DefaultEditor.ini)

```ini
   1: [/Script/AdvancedPreviewScene.SharedProfiles]
   2: +Profiles=(ProfileName="Epic Headquarters",bSharedProfile=True,bIsEngineDefaultProfile=True,bUseSkyLighting=True,DirectionalLightIntensity=1.000000,DirectionalLightColor=(R=1.000000,G=1.000000,B=1.000000,A=1.000000),SkyLightIntensity=1.000000,bRotateLightingRig=False,bShowEnvironment=True,bShowFloor=True,bShowGrid=False,EnvironmentColor=(R=0.200000,G=0.200000,B=0.200000,A=1.000000),EnvironmentIntensity=1.000000,EnvironmentCubeMapPath="/Engine/EditorMaterials/AssetViewer/EpicQuadPanorama_CC+EV1.EpicQuadPanorama_CC+EV1",bPostProcessingEnabled=True,PostProcessingSettings=(bOverride_TemperatureType=False,bOverride_WhiteTemp=False,bOverride_WhiteTint=False,bOverride_ColorSaturation=False,bOverride_ColorContrast=False,bOverride_ColorGamma=False,bOverride_ColorGain=False,bOverride_ColorOffset=False,bOverride_ColorSaturationShadows=False,bOverride_ColorContrastShadows=False,bOverride_ColorGammaShadows=False,bOverride_ColorGainShadows=False,bOverride_ColorOffsetShadows=False,bOverride_ColorSaturationMidtones=False,bOverride_ColorContrastMidtones=False,bOverride_ColorGammaMidtones=False,bOverride_ColorGainMidtones=False,bOverride_ColorOffsetMidtones=False,bOverride_ColorSaturationHighlights=False,bOverride_ColorContrastHighlights=False,bOverride_ColorGammaHighlights=False,bOverride_ColorGainHighlights=False,bOverride_ColorOffsetHighlights=False,bOverride_ColorCorrectionShadowsMax=False,bOverride_ColorCorrectionHighlightsMin=False,bOverride_ColorCorrectionHighlightsMax=False,bOverride_BlueCorrection=False,bOverride_ExpandGamut=False,bOverride_ToneCurveAmount=False,bOverride_FilmSlope=False,bOverride_FilmToe=False,bOverride_FilmShoulder=False,bOverride_FilmBlackClip=False,bOverride_FilmWhiteClip=False,bOverride_SceneColorTint=False,bOverride_SceneFringeIntensity=False,bOverride_ChromaticAberrationStartOffset=False,bOverride_bMegaLights=False,bOverride_AmbientCubemapTint=False,bOverride_AmbientCubemapIntensity=False,bOverride_BloomMethod=False,bOverride_BloomIntensity=False,bOverride_BloomThreshold=False,bOverride_Bloom1Tint=False,bOverride_Bloom1Size=False,bOverride_Bloom2Size=False,bOverride_Bloom2Tint=False,bOverride_Bloom3Tint=False,bOverride_Bloom3Size=False,bOverride_Bloom4Tint=False,bOverride_Bloom4Size=False,bOverride_Bloom5Tint=False,bOverride_Bloom5Size=False,bOverride_Bloom6Tint=False,bOverride_Bloom6Size=False,bOverride_BloomSizeScale=False,bOverride_BloomConvolutionTexture=False,bOverride_BloomConvolutionScatterDispersion=False,bOverride_BloomConvolutionSize=False,bOverride_BloomConvolutionCenterUV=False,bOverride_BloomConvolutionPreFilterMin=False,bOverride_BloomConvolutionPreFilterMax=False,bOverride_BloomConvolutionPreFilterMult=False,bOverride_BloomConvolutionBufferScale=False,bOverride_BloomDirtMaskIntensity=False,bOverride_BloomDirtMaskTint=False,bOverride_BloomDirtMask=False,bOverride_CameraShutterSpeed=False,bOverride_CameraISO=False,bOverride_AutoExposureMethod=False,bOverride_AutoExposureLowPercent=False,bOverride_AutoExposureHighPercent=False,bOverride_AutoExposureMinBrightness=False,bOverride_AutoExposureMaxBrightness=False,bOverride_AutoExposureSpeedUp=False,bOverride_AutoExposureSpeedDown=False,bOverride_AutoExposureBias=False,bOverride_AutoExposureBiasCurve=False,bOverride_AutoExposureMeterMask=False,bOverride_AutoExposureApplyPhysicalCameraExposure=False,bOverride_HistogramLogMin=False,bOverride_HistogramLogMax=False,bOverride_LocalExposureMethod=False,bOverride_LocalExposureHighlightContrastScale=False,bOverride_LocalExposureShadowContrastScale=False,bOverride_LocalExposureHighlightContrastCurve=False,bOverride_LocalExposureShadowContrastCurve=False,bOverride_LocalExposureHighlightThreshold=False,bOverride_LocalExposureShadowThreshold=False,bOverride_LocalExposureDetailStrength=False,bOverride_LocalExposureBlurredLuminanceBlend=False,bOverride_LocalExposureBlurredLuminanceKernelSizePercent=False,bOverride_LocalExposureMiddleGreyBias=False,bOverride_LensFlareIntensity=False,bOverride_LensFlareTint=False,bOverride_LensFlareTints=False,bOverride_LensFlareBokehSize=False,bOverride_LensFlareBokehShape=False,bOverride_LensFlareThreshold=False,bOverride_VignetteIntensity=False,bOverride_Sharpen=False,bOverride_FilmGrainIntensity=False,bOverride_FilmGrainIntensityShadows=False,bOverride_FilmGrainIntensityMidtones=False,bOverride_FilmGrainIntensityHighlights=False,bOverride_FilmGrainShadowsMax=False,bOverride_FilmGrainHighlightsMin=False,bOverride_FilmGrainHighlightsMax=False,bOverride_FilmGrainTexelSize=False,bOverride_FilmGrainTexture=False,bOverride_AmbientOcclusionIntensity=False,bOverride_AmbientOcclusionStaticFraction=False,bOverride_AmbientOcclusionRadius=False,bOverride_AmbientOcclusionFadeDistance=False,bOverride_AmbientOcclusionFadeRadius=False,bOverride_AmbientOcclusionRadiusInWS=False,bOverride_AmbientOcclusionPower=False,bOverride_AmbientOcclusionBias=False,bOverride_AmbientOcclusionQuality=False,bOverride_AmbientOcclusionMipBlend=False,bOverride_AmbientOcclusionMipScale=False,bOverride_AmbientOcclusionMipThreshold=False,bOverride_AmbientOcclusionTemporalBlendWeight=False,bOverride_RayTracingAO=False,bOverride_RayTracingAOSamplesPerPixel=False,bOverride_RayTracingAOIntensity=False,bOverride_RayTracingAORadius=False,bOverride_IndirectLightingColor=False,bOverride_IndirectLightingIntensity=False,bOverride_ColorGradingIntensity=False,bOverride_ColorGradingLUT=False,bOverride_DepthOfFieldFocalDistance=False,bOverride_DepthOfFieldFstop=False,bOverride_DepthOfFieldMinFstop=False,bOverride_DepthOfFieldBladeCount=False,bOverride_DepthOfFieldSensorWidth=False,bOverride_DepthOfFieldSqueezeFactor=False,bOverride_DepthOfFieldDepthBlurRadius=False,bOverride_DepthOfFieldUseHairDepth=False,bOverride_DepthOfFieldDepthBlurAmount=False,bOverride_DepthOfFieldFocalRegion=False,bOverride_DepthOfFieldNearTransitionRegion=False,bOverride_DepthOfFieldFarTransitionRegion=False,bOverride_DepthOfFieldScale=False,bOverride_DepthOfFieldNearBlurSize=False,bOverride_DepthOfFieldFarBlurSize=False,bOverride_MobileHQGaussian=False,bOverride_DepthOfFieldOcclusion=False,bOverride_DepthOfFieldSkyFocusDistance=False,bOverride_DepthOfFieldVignetteSize=False,bOverride_MotionBlurAmount=False,bOverride_MotionBlurMax=False,bOverride_MotionBlurTargetFPS=False,bOverride_MotionBlurPerObjectSize=False,bOverride_ReflectionMethod=False,bOverride_LumenReflectionQuality=False,bOverride_ScreenSpaceReflectionIntensity=False,bOverride_ScreenSpaceReflectionQuality=False,bOverride_ScreenSpaceReflectionMaxRoughness=False,bOverride_ScreenSpaceReflectionRoughnessScale=False,bOverride_UserFlags=False,bOverride_RayTracingReflectionsMaxRoughness=False,bOverride_RayTracingReflectionsMaxBounces=False,bOverride_RayTracingReflectionsSamplesPerPixel=False,bOverride_RayTracingReflectionsShadows=False,bOverride_RayTracingReflectionsTranslucency=False,bOverride_TranslucencyType=False,bOverride_RayTracingTranslucencyMaxRoughness=False,bOverride_RayTracingTranslucencyRefractionRays=False,bOverride_RayTracingTranslucencySamplesPerPixel=False,bOverride_RayTracingTranslucencyShadows=False,bOverride_RayTracingTranslucencyRefraction=False,bOverride_DynamicGlobalIlluminationMethod=False,bOverride_LumenSceneLightingQuality=False,bOverride_LumenSceneDetail=False,bOverride_LumenSceneViewDistance=False,bOverride_LumenSceneLightingUpdateSpeed=False,bOverride_LumenFinalGatherQuality=False,bOverride_LumenFinalGatherLightingUpdateSpeed=False,bOverride_LumenFinalGatherScreenTraces=False,bOverride_LumenMaxTraceDistance=False,bOverride_LumenDiffuseColorBoost=False,bOverride_LumenSkylightLeaking=False,bOverride_LumenFullSkylightLeakingDistance=False,bOverride_LumenRayLightingMode=False,bOverride_LumenReflectionsScreenTraces=False,bOverride_LumenFrontLayerTranslucencyReflections=False,bOverride_LumenMaxRoughnessToTraceReflections=False,bOverride_LumenMaxReflectionBounces=False,bOverride_LumenMaxRefractionBounces=False,bOverride_LumenSurfaceCacheResolution=False,bOverride_RayTracingGI=False,bOverride_RayTracingGIMaxBounces=False,bOverride_RayTracingGISamplesPerPixel=False,bOverride_PathTracingMaxBounces=False,bOverride_PathTracingSamplesPerPixel=False,bOverride_PathTracingMaxPathIntensity=False,bOverride_PathTracingEnableEmissiveMaterials=False,bOverride_PathTracingEnableReferenceDOF=False,bOverride_PathTracingEnableReferenceAtmosphere=False,bOverride_PathTracingEnableDenoiser=False,bOverride_PathTracingIncludeEmissive=False,bOverride_PathTracingIncludeDiffuse=False,bOverride_PathTracingIncludeIndirectDiffuse=False,bOverride_PathTracingIncludeSpecular=False,bOverride_PathTracingIncludeIndirectSpecular=False,bOverride_PathTracingIncludeVolume=False,bOverride_PathTracingIncludeIndirectVolume=False,bMobileHQGaussian=False,BloomMethod=BM_SOG,AutoExposureMethod=AEM_Histogram,TemperatureType=TEMP_WhiteBalance,WhiteTemp=6500.000000,WhiteTint=0.000000,ColorSaturation=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorContrast=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorGamma=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorGain=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorOffset=(X=0.000000,Y=0.000000,Z=0.000000,W=0.000000),ColorSaturationShadows=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorContrastShadows=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorGammaShadows=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorGainShadows=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorOffsetShadows=(X=0.000000,Y=0.000000,Z=0.000000,W=0.000000),ColorSaturationMidtones=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorContrastMidtones=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorGammaMidtones=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorGainMidtones=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorOffsetMidtones=(X=0.000000,Y=0.000000,Z=0.000000,W=0.000000),ColorSaturationHighlights=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorContrastHighlights=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorGammaHighlights=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorGainHighlights=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorOffsetHighlights=(X=0.000000,Y=0.000000,Z=0.000000,W=0.000000),ColorCorrectionHighlightsMin=0.500000,ColorCorrectionHighlightsMax=1.000000,ColorCorrectionShadowsMax=0.090000,BlueCorrection=0.600000,ExpandGamut=1.000000,ToneCurveAmount=1.000000,FilmSlope=0.880000,FilmToe=0.550000,FilmShoulder=0.260000,FilmBlackClip=0.000000,FilmWhiteClip=0.040000,SceneColorTint=(R=1.000000,G=1.000000,B=1.000000,A=1.000000),SceneFringeIntensity=0.000000,ChromaticAberrationStartOffset=0.000000,BloomIntensity=0.675000,BloomThreshold=-1.000000,BloomSizeScale=4.000000,Bloom1Size=0.300000,Bloom2Size=1.000000,Bloom3Size=2.000000,Bloom4Size=10.000000,Bloom5Size=30.000000,Bloom6Size=64.000000,Bloom1Tint=(R=0.346500,G=0.346500,B=0.346500,A=1.000000),Bloom2Tint=(R=0.138000,G=0.138000,B=0.138000,A=1.000000),Bloom3Tint=(R=0.117600,G=0.117600,B=0.117600,A=1.000000),Bloom4Tint=(R=0.066000,G=0.066000,B=0.066000,A=1.000000),Bloom5Tint=(R=0.066000,G=0.066000,B=0.066000,A=1.000000),Bloom6Tint=(R=0.061000,G=0.061000,B=0.061000,A=1.000000),BloomConvolutionScatterDispersion=1.000000,BloomConvolutionSize=1.000000,BloomConvolutionTexture=None,BloomConvolutionCenterUV=(X=0.500000,Y=0.500000),BloomConvolutionPreFilterMin=7.000000,BloomConvolutionPreFilterMax=15000.000000,BloomConvolutionPreFilterMult=15.000000,BloomConvolutionBufferScale=0.133000,BloomDirtMask=None,BloomDirtMaskIntensity=0.000000,BloomDirtMaskTint=(R=0.500000,G=0.500000,B=0.500000,A=1.000000),DynamicGlobalIlluminationMethod=Lumen,IndirectLightingColor=(R=1.000000,G=1.000000,B=1.000000,A=1.000000),IndirectLightingIntensity=1.000000,LumenRayLightingMode=Default,LumenSceneLightingQuality=1.000000,LumenSceneDetail=1.000000,LumenSceneViewDistance=20000.000000,LumenSceneLightingUpdateSpeed=1.000000,LumenFinalGatherQuality=1.000000,LumenFinalGatherLightingUpdateSpeed=1.000000,LumenFinalGatherScreenTraces=True,LumenMaxTraceDistance=20000.000000,LumenDiffuseColorBoost=1.000000,LumenSkylightLeaking=0.000000,LumenFullSkylightLeakingDistance=1000.000000,LumenSurfaceCacheResolution=1.000000,ReflectionMethod=Lumen,LumenReflectionQuality=1.000000,LumenReflectionsScreenTraces=True,LumenFrontLayerTranslucencyReflections=False,LumenMaxRoughnessToTraceReflections=0.400000,LumenMaxReflectionBounces=1,LumenMaxRefractionBounces=0,ScreenSpaceReflectionIntensity=100.000000,ScreenSpaceReflectionQuality=50.000000,ScreenSpaceReflectionMaxRoughness=0.600000,bMegaLights=True,AmbientCubemapTint=(R=1.000000,G=1.000000,B=1.000000,A=1.000000),AmbientCubemapIntensity=1.000000,AmbientCubemap=None,CameraShutterSpeed=60.000000,CameraISO=100.000000,DepthOfFieldFstop=4.000000,DepthOfFieldMinFstop=1.200000,DepthOfFieldBladeCount=5,AutoExposureBias=1.000000,AutoExposureBiasBackup=0.000000,bOverride_AutoExposureBiasBackup=False,AutoExposureApplyPhysicalCameraExposure=True,AutoExposureBiasCurve=None,AutoExposureMeterMask=None,AutoExposureLowPercent=10.000000,AutoExposureHighPercent=90.000000,AutoExposureMinBrightness=-10.000000,AutoExposureMaxBrightness=20.000000,AutoExposureSpeedUp=3.000000,AutoExposureSpeedDown=1.000000,HistogramLogMin=-10.000000,HistogramLogMax=20.000000,LocalExposureMethod=Bilateral,LocalExposureHighlightContrastScale=1.000000,LocalExposureShadowContrastScale=1.000000,LocalExposureHighlightContrastCurve=None,LocalExposureShadowContrastCurve=None,LocalExposureHighlightThreshold=0.000000,LocalExposureShadowThreshold=0.000000,LocalExposureDetailStrength=1.000000,LocalExposureBlurredLuminanceBlend=0.600000,LocalExposureBlurredLuminanceKernelSizePercent=50.000000,LocalExposureMiddleGreyBias=0.000000,LensFlareIntensity=1.000000,LensFlareTint=(R=1.000000,G=1.000000,B=1.000000,A=1.000000),LensFlareBokehSize=3.000000,LensFlareThreshold=8.000000,LensFlareBokehShape=None,LensFlareTints[0]=(R=1.000000,G=0.800000,B=0.400000,A=0.600000),LensFlareTints[1]=(R=1.000000,G=1.000000,B=0.600000,A=0.530000),LensFlareTints[2]=(R=0.800000,G=0.800000,B=1.000000,A=0.460000),LensFlareTints[3]=(R=0.500000,G=1.000000,B=0.400000,A=0.390000),LensFlareTints[4]=(R=0.500000,G=0.800000,B=1.000000,A=0.310000),LensFlareTints[5]=(R=0.900000,G=1.000000,B=0.800000,A=0.270000),LensFlareTints[6]=(R=1.000000,G=0.800000,B=0.400000,A=0.220000),LensFlareTints[7]=(R=0.900000,G=0.700000,B=0.700000,A=0.150000),VignetteIntensity=0.400000,Sharpen=0.000000,FilmGrainIntensity=0.000000,FilmGrainIntensityShadows=1.000000,FilmGrainIntensityMidtones=1.000000,FilmGrainIntensityHighlights=1.000000,FilmGrainShadowsMax=0.090000,FilmGrainHighlightsMin=0.500000,FilmGrainHighlightsMax=1.000000,FilmGrainTexelSize=1.000000,FilmGrainTexture=None,AmbientOcclusionIntensity=0.500000,AmbientOcclusionStaticFraction=1.000000,AmbientOcclusionRadius=200.000000,AmbientOcclusionRadiusInWS=False,AmbientOcclusionFadeDistance=8000.000000,AmbientOcclusionFadeRadius=5000.000000,AmbientOcclusionPower=2.000000,AmbientOcclusionBias=3.000000,AmbientOcclusionQuality=50.000000,AmbientOcclusionMipBlend=0.600000,AmbientOcclusionMipScale=1.700000,AmbientOcclusionMipThreshold=0.010000,AmbientOcclusionTemporalBlendWeight=0.100000,RayTracingAO=False,RayTracingAOSamplesPerPixel=1,RayTracingAOIntensity=1.000000,RayTracingAORadius=200.000000,ColorGradingIntensity=1.000000,ColorGradingLUT=None,DepthOfFieldSensorWidth=24.576000,DepthOfFieldSqueezeFactor=1.000000,DepthOfFieldFocalDistance=0.000000,DepthOfFieldDepthBlurAmount=1.000000,DepthOfFieldDepthBlurRadius=0.000000,DepthOfFieldUseHairDepth=False,DepthOfFieldFocalRegion=0.000000,DepthOfFieldNearTransitionRegion=300.000000,DepthOfFieldFarTransitionRegion=500.000000,DepthOfFieldScale=0.000000,DepthOfFieldNearBlurSize=15.000000,DepthOfFieldFarBlurSize=15.000000,DepthOfFieldOcclusion=0.400000,DepthOfFieldSkyFocusDistance=0.000000,DepthOfFieldVignetteSize=200.000000,MotionBlurAmount=0.500000,MotionBlurMax=5.000000,MotionBlurTargetFPS=30,MotionBlurPerObjectSize=0.000000,TranslucencyType=Raster,RayTracingTranslucencyMaxRoughness=0.600000,RayTracingTranslucencyRefractionRays=3,RayTracingTranslucencySamplesPerPixel=1,RayTracingTranslucencyShadows=Hard_shadows,RayTracingTranslucencyRefraction=True,PathTracingMaxBounces=32,PathTracingSamplesPerPixel=2048,PathTracingMaxPathIntensity=24.000000,PathTracingEnableEmissiveMaterials=True,PathTracingEnableReferenceDOF=False,PathTracingEnableReferenceAtmosphere=False,PathTracingEnableDenoiser=True,PathTracingIncludeEmissive=True,PathTracingIncludeDiffuse=True,PathTracingIncludeIndirectDiffuse=True,PathTracingIncludeSpecular=True,PathTracingIncludeIndirectSpecular=True,PathTracingIncludeVolume=True,PathTracingIncludeIndirectVolume=True,UserFlags=0,WeightedBlendables=(Array=)),LightingRigRotation=0.000000,RotationSpeed=2.000000,DirectionalLightRotation=(Pitch=-40.000000,Yaw=-67.500000,Roll=0.000000),bEnableToneMapping=True,bShowMeshEdges=False)
   3: +Profiles=(ProfileName="Grey Wireframe",bSharedProfile=True,bIsEngineDefaultProfile=True,bUseSkyLighting=True,DirectionalLightIntensity=1.000000,DirectionalLightColor=(R=1.000000,G=1.000000,B=1.000000,A=1.000000),SkyLightIntensity=1.000000,bRotateLightingRig=False,bShowEnvironment=False,bShowFloor=False,bShowGrid=True,EnvironmentColor=(R=0.039216,G=0.039216,B=0.039216,A=1.000000),EnvironmentIntensity=1.000000,EnvironmentCubeMapPath="/Engine/EditorMaterials/AssetViewer/EpicQuadPanorama_CC+EV1.EpicQuadPanorama_CC+EV1",bPostProcessingEnabled=False,PostProcessingSettings=(bOverride_TemperatureType=False,bOverride_WhiteTemp=False,bOverride_WhiteTint=False,bOverride_ColorSaturation=False,bOverride_ColorContrast=False,bOverride_ColorGamma=False,bOverride_ColorGain=False,bOverride_ColorOffset=False,bOverride_ColorSaturationShadows=False,bOverride_ColorContrastShadows=False,bOverride_ColorGammaShadows=False,bOverride_ColorGainShadows=False,bOverride_ColorOffsetShadows=False,bOverride_ColorSaturationMidtones=False,bOverride_ColorContrastMidtones=False,bOverride_ColorGammaMidtones=False,bOverride_ColorGainMidtones=False,bOverride_ColorOffsetMidtones=False,bOverride_ColorSaturationHighlights=False,bOverride_ColorContrastHighlights=False,bOverride_ColorGammaHighlights=False,bOverride_ColorGainHighlights=False,bOverride_ColorOffsetHighlights=False,bOverride_ColorCorrectionShadowsMax=False,bOverride_ColorCorrectionHighlightsMin=False,bOverride_ColorCorrectionHighlightsMax=False,bOverride_BlueCorrection=False,bOverride_ExpandGamut=False,bOverride_ToneCurveAmount=False,bOverride_FilmSlope=False,bOverride_FilmToe=False,bOverride_FilmShoulder=False,bOverride_FilmBlackClip=False,bOverride_FilmWhiteClip=False,bOverride_SceneColorTint=False,bOverride_SceneFringeIntensity=False,bOverride_ChromaticAberrationStartOffset=False,bOverride_bMegaLights=False,bOverride_AmbientCubemapTint=False,bOverride_AmbientCubemapIntensity=False,bOverride_BloomMethod=False,bOverride_BloomIntensity=False,bOverride_BloomThreshold=False,bOverride_Bloom1Tint=False,bOverride_Bloom1Size=False,bOverride_Bloom2Size=False,bOverride_Bloom2Tint=False,bOverride_Bloom3Tint=False,bOverride_Bloom3Size=False,bOverride_Bloom4Tint=False,bOverride_Bloom4Size=False,bOverride_Bloom5Tint=False,bOverride_Bloom5Size=False,bOverride_Bloom6Tint=False,bOverride_Bloom6Size=False,bOverride_BloomSizeScale=False,bOverride_BloomConvolutionTexture=False,bOverride_BloomConvolutionScatterDispersion=False,bOverride_BloomConvolutionSize=False,bOverride_BloomConvolutionCenterUV=False,bOverride_BloomConvolutionPreFilterMin=False,bOverride_BloomConvolutionPreFilterMax=False,bOverride_BloomConvolutionPreFilterMult=False,bOverride_BloomConvolutionBufferScale=False,bOverride_BloomDirtMaskIntensity=False,bOverride_BloomDirtMaskTint=False,bOverride_BloomDirtMask=False,bOverride_CameraShutterSpeed=False,bOverride_CameraISO=False,bOverride_AutoExposureMethod=False,bOverride_AutoExposureLowPercent=False,bOverride_AutoExposureHighPercent=False,bOverride_AutoExposureMinBrightness=False,bOverride_AutoExposureMaxBrightness=False,bOverride_AutoExposureSpeedUp=False,bOverride_AutoExposureSpeedDown=False,bOverride_AutoExposureBias=False,bOverride_AutoExposureBiasCurve=False,bOverride_AutoExposureMeterMask=False,bOverride_AutoExposureApplyPhysicalCameraExposure=False,bOverride_HistogramLogMin=False,bOverride_HistogramLogMax=False,bOverride_LocalExposureMethod=False,bOverride_LocalExposureHighlightContrastScale=False,bOverride_LocalExposureShadowContrastScale=False,bOverride_LocalExposureHighlightContrastCurve=False,bOverride_LocalExposureShadowContrastCurve=False,bOverride_LocalExposureHighlightThreshold=False,bOverride_LocalExposureShadowThreshold=False,bOverride_LocalExposureDetailStrength=False,bOverride_LocalExposureBlurredLuminanceBlend=False,bOverride_LocalExposureBlurredLuminanceKernelSizePercent=False,bOverride_LocalExposureMiddleGreyBias=False,bOverride_LensFlareIntensity=False,bOverride_LensFlareTint=False,bOverride_LensFlareTints=False,bOverride_LensFlareBokehSize=False,bOverride_LensFlareBokehShape=False,bOverride_LensFlareThreshold=False,bOverride_VignetteIntensity=False,bOverride_Sharpen=False,bOverride_FilmGrainIntensity=False,bOverride_FilmGrainIntensityShadows=False,bOverride_FilmGrainIntensityMidtones=False,bOverride_FilmGrainIntensityHighlights=False,bOverride_FilmGrainShadowsMax=False,bOverride_FilmGrainHighlightsMin=False,bOverride_FilmGrainHighlightsMax=False,bOverride_FilmGrainTexelSize=False,bOverride_FilmGrainTexture=False,bOverride_AmbientOcclusionIntensity=False,bOverride_AmbientOcclusionStaticFraction=False,bOverride_AmbientOcclusionRadius=False,bOverride_AmbientOcclusionFadeDistance=False,bOverride_AmbientOcclusionFadeRadius=False,bOverride_AmbientOcclusionRadiusInWS=False,bOverride_AmbientOcclusionPower=False,bOverride_AmbientOcclusionBias=False,bOverride_AmbientOcclusionQuality=False,bOverride_AmbientOcclusionMipBlend=False,bOverride_AmbientOcclusionMipScale=False,bOverride_AmbientOcclusionMipThreshold=False,bOverride_AmbientOcclusionTemporalBlendWeight=False,bOverride_RayTracingAO=False,bOverride_RayTracingAOSamplesPerPixel=False,bOverride_RayTracingAOIntensity=False,bOverride_RayTracingAORadius=False,bOverride_IndirectLightingColor=False,bOverride_IndirectLightingIntensity=False,bOverride_ColorGradingIntensity=False,bOverride_ColorGradingLUT=False,bOverride_DepthOfFieldFocalDistance=False,bOverride_DepthOfFieldFstop=False,bOverride_DepthOfFieldMinFstop=False,bOverride_DepthOfFieldBladeCount=False,bOverride_DepthOfFieldSensorWidth=False,bOverride_DepthOfFieldSqueezeFactor=False,bOverride_DepthOfFieldDepthBlurRadius=False,bOverride_DepthOfFieldUseHairDepth=False,bOverride_DepthOfFieldDepthBlurAmount=False,bOverride_DepthOfFieldFocalRegion=False,bOverride_DepthOfFieldNearTransitionRegion=False,bOverride_DepthOfFieldFarTransitionRegion=False,bOverride_DepthOfFieldScale=False,bOverride_DepthOfFieldNearBlurSize=False,bOverride_DepthOfFieldFarBlurSize=False,bOverride_MobileHQGaussian=False,bOverride_DepthOfFieldOcclusion=False,bOverride_DepthOfFieldSkyFocusDistance=False,bOverride_DepthOfFieldVignetteSize=False,bOverride_MotionBlurAmount=False,bOverride_MotionBlurMax=False,bOverride_MotionBlurTargetFPS=False,bOverride_MotionBlurPerObjectSize=False,bOverride_ReflectionMethod=False,bOverride_LumenReflectionQuality=False,bOverride_ScreenSpaceReflectionIntensity=False,bOverride_ScreenSpaceReflectionQuality=False,bOverride_ScreenSpaceReflectionMaxRoughness=False,bOverride_ScreenSpaceReflectionRoughnessScale=False,bOverride_UserFlags=False,bOverride_RayTracingReflectionsMaxRoughness=False,bOverride_RayTracingReflectionsMaxBounces=False,bOverride_RayTracingReflectionsSamplesPerPixel=False,bOverride_RayTracingReflectionsShadows=False,bOverride_RayTracingReflectionsTranslucency=False,bOverride_TranslucencyType=False,bOverride_RayTracingTranslucencyMaxRoughness=False,bOverride_RayTracingTranslucencyRefractionRays=False,bOverride_RayTracingTranslucencySamplesPerPixel=False,bOverride_RayTracingTranslucencyShadows=False,bOverride_RayTracingTranslucencyRefraction=False,bOverride_DynamicGlobalIlluminationMethod=False,bOverride_LumenSceneLightingQuality=False,bOverride_LumenSceneDetail=False,bOverride_LumenSceneViewDistance=False,bOverride_LumenSceneLightingUpdateSpeed=False,bOverride_LumenFinalGatherQuality=False,bOverride_LumenFinalGatherLightingUpdateSpeed=False,bOverride_LumenFinalGatherScreenTraces=False,bOverride_LumenMaxTraceDistance=False,bOverride_LumenDiffuseColorBoost=False,bOverride_LumenSkylightLeaking=False,bOverride_LumenFullSkylightLeakingDistance=False,bOverride_LumenRayLightingMode=False,bOverride_LumenReflectionsScreenTraces=False,bOverride_LumenFrontLayerTranslucencyReflections=False,bOverride_LumenMaxRoughnessToTraceReflections=False,bOverride_LumenMaxReflectionBounces=False,bOverride_LumenMaxRefractionBounces=False,bOverride_LumenSurfaceCacheResolution=False,bOverride_RayTracingGI=False,bOverride_RayTracingGIMaxBounces=False,bOverride_RayTracingGISamplesPerPixel=False,bOverride_PathTracingMaxBounces=False,bOverride_PathTracingSamplesPerPixel=False,bOverride_PathTracingMaxPathIntensity=False,bOverride_PathTracingEnableEmissiveMaterials=False,bOverride_PathTracingEnableReferenceDOF=False,bOverride_PathTracingEnableReferenceAtmosphere=False,bOverride_PathTracingEnableDenoiser=False,bOverride_PathTracingIncludeEmissive=False,bOverride_PathTracingIncludeDiffuse=False,bOverride_PathTracingIncludeIndirectDiffuse=False,bOverride_PathTracingIncludeSpecular=False,bOverride_PathTracingIncludeIndirectSpecular=False,bOverride_PathTracingIncludeVolume=False,bOverride_PathTracingIncludeIndirectVolume=False,bMobileHQGaussian=False,BloomMethod=BM_SOG,AutoExposureMethod=AEM_Histogram,TemperatureType=TEMP_WhiteBalance,WhiteTemp=6500.000000,WhiteTint=0.000000,ColorSaturation=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorContrast=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorGamma=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorGain=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorOffset=(X=0.000000,Y=0.000000,Z=0.000000,W=0.000000),ColorSaturationShadows=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorContrastShadows=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorGammaShadows=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorGainShadows=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorOffsetShadows=(X=0.000000,Y=0.000000,Z=0.000000,W=0.000000),ColorSaturationMidtones=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorContrastMidtones=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorGammaMidtones=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorGainMidtones=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorOffsetMidtones=(X=0.000000,Y=0.000000,Z=0.000000,W=0.000000),ColorSaturationHighlights=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorContrastHighlights=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorGammaHighlights=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorGainHighlights=(X=1.000000,Y=1.000000,Z=1.000000,W=1.000000),ColorOffsetHighlights=(X=0.000000,Y=0.000000,Z=0.000000,W=0.000000),ColorCorrectionHighlightsMin=0.500000,ColorCorrectionHighlightsMax=1.000000,ColorCorrectionShadowsMax=0.090000,BlueCorrection=0.600000,ExpandGamut=1.000000,ToneCurveAmount=1.000000,FilmSlope=0.880000,FilmToe=0.550000,FilmShoulder=0.260000,FilmBlackClip=0.000000,FilmWhiteClip=0.040000,SceneColorTint=(R=1.000000,G=1.000000,B=1.000000,A=1.000000),SceneFringeIntensity=0.000000,ChromaticAberrationStartOffset=0.000000,BloomIntensity=0.675000,BloomThreshold=-1.000000,BloomSizeScale=4.000000,Bloom1Size=0.300000,Bloom2Size=1.000000,Bloom3Size=2.000000,Bloom4Size=10.000000,Bloom5Size=30.000000,Bloom6Size=64.000000,Bloom1Tint=(R=0.346500,G=0.346500,B=0.346500,A=1.000000),Bloom2Tint=(R=0.138000,G=0.138000,B=0.138000,A=1.000000),Bloom3Tint=(R=0.117600,G=0.117600,B=0.117600,A=1.000000),Bloom4Tint=(R=0.066000,G=0.066000,B=0.066000,A=1.000000),Bloom5Tint=(R=0.066000,G=0.066000,B=0.066000,A=1.000000),Bloom6Tint=(R=0.061000,G=0.061000,B=0.061000,A=1.000000),BloomConvolutionScatterDispersion=1.000000,BloomConvolutionSize=1.000000,BloomConvolutionTexture=None,BloomConvolutionCenterUV=(X=0.500000,Y=0.500000),BloomConvolutionPreFilterMin=7.000000,BloomConvolutionPreFilterMax=15000.000000,BloomConvolutionPreFilterMult=15.000000,BloomConvolutionBufferScale=0.133000,BloomDirtMask=None,BloomDirtMaskIntensity=0.000000,BloomDirtMaskTint=(R=0.500000,G=0.500000,B=0.500000,A=1.000000),DynamicGlobalIlluminationMethod=Lumen,IndirectLightingColor=(R=1.000000,G=1.000000,B=1.000000,A=1.000000),IndirectLightingIntensity=1.000000,LumenRayLightingMode=Default,LumenSceneLightingQuality=1.000000,LumenSceneDetail=1.000000,LumenSceneViewDistance=20000.000000,LumenSceneLightingUpdateSpeed=1.000000,LumenFinalGatherQuality=1.000000,LumenFinalGatherLightingUpdateSpeed=1.000000,LumenFinalGatherScreenTraces=True,LumenMaxTraceDistance=20000.000000,LumenDiffuseColorBoost=1.000000,LumenSkylightLeaking=0.000000,LumenFullSkylightLeakingDistance=1000.000000,LumenSurfaceCacheResolution=1.000000,ReflectionMethod=Lumen,LumenReflectionQuality=1.000000,LumenReflectionsScreenTraces=True,LumenFrontLayerTranslucencyReflections=False,LumenMaxRoughnessToTraceReflections=0.400000,LumenMaxReflectionBounces=1,LumenMaxRefractionBounces=0,ScreenSpaceReflectionIntensity=100.000000,ScreenSpaceReflectionQuality=50.000000,ScreenSpaceReflectionMaxRoughness=0.600000,bMegaLights=True,AmbientCubemapTint=(R=1.000000,G=1.000000,B=1.000000,A=1.000000),AmbientCubemapIntensity=1.000000,AmbientCubemap=None,CameraShutterSpeed=60.000000,CameraISO=100.000000,DepthOfFieldFstop=4.000000,DepthOfFieldMinFstop=1.200000,DepthOfFieldBladeCount=5,AutoExposureBias=1.000000,AutoExposureBiasBackup=0.000000,bOverride_AutoExposureBiasBackup=False,AutoExposureApplyPhysicalCameraExposure=True,AutoExposureBiasCurve=None,AutoExposureMeterMask=None,AutoExposureLowPercent=10.000000,AutoExposureHighPercent=90.000000,AutoExposureMinBrightness=-10.000000,AutoExposureMaxBrightness=20.000000,AutoExposureSpeedUp=3.000000,AutoExposureSpeedDown=1.000000,HistogramLogMin=-10.000000,HistogramLogMax=20.000000,LocalExposureMethod=Bilateral,LocalExposureHighlightContrastScale=1.000000,LocalExposureShadowContrastScale=1.000000,LocalExposureHighlightContrastCurve=None,LocalExposureShadowContrastCurve=None,LocalExposureHighlightThreshold=0.000000,LocalExposureShadowThreshold=0.000000,LocalExposureDetailStrength=1.000000,LocalExposureBlurredLuminanceBlend=0.600000,LocalExposureBlurredLuminanceKernelSizePercent=50.000000,LocalExposureMiddleGreyBias=0.000000,LensFlareIntensity=1.000000,LensFlareTint=(R=1.000000,G=1.000000,B=1.000000,A=1.000000),LensFlareBokehSize=3.000000,LensFlareThreshold=8.000000,LensFlareBokehShape=None,LensFlareTints[0]=(R=1.000000,G=0.800000,B=0.400000,A=0.600000),LensFlareTints[1]=(R=1.000000,G=1.000000,B=0.600000,A=0.530000),LensFlareTints[2]=(R=0.800000,G=0.800000,B=1.000000,A=0.460000),LensFlareTints[3]=(R=0.500000,G=1.000000,B=0.400000,A=0.390000),LensFlareTints[4]=(R=0.500000,G=0.800000,B=1.000000,A=0.310000),LensFlareTints[5]=(R=0.900000,G=1.000000,B=0.800000,A=0.270000),LensFlareTints[6]=(R=1.000000,G=0.800000,B=0.400000,A=0.220000),LensFlareTints[7]=(R=0.900000,G=0.700000,B=0.700000,A=0.150000),VignetteIntensity=0.400000,Sharpen=0.000000,FilmGrainIntensity=0.000000,FilmGrainIntensityShadows=1.000000,FilmGrainIntensityMidtones=1.000000,FilmGrainIntensityHighlights=1.000000,FilmGrainShadowsMax=0.090000,FilmGrainHighlightsMin=0.500000,FilmGrainHighlightsMax=1.000000,FilmGrainTexelSize=1.000000,FilmGrainTexture=None,AmbientOcclusionIntensity=0.500000,AmbientOcclusionStaticFraction=1.000000,AmbientOcclusionRadius=200.000000,AmbientOcclusionRadiusInWS=False,AmbientOcclusionFadeDistance=8000.000000,AmbientOcclusionFadeRadius=5000.000000,AmbientOcclusionPower=2.000000,AmbientOcclusionBias=3.000000,AmbientOcclusionQuality=50.000000,AmbientOcclusionMipBlend=0.600000,AmbientOcclusionMipScale=1.700000,AmbientOcclusionMipThreshold=0.010000,AmbientOcclusionTemporalBlendWeight=0.100000,RayTracingAO=False,RayTracingAOSamplesPerPixel=1,RayTracingAOIntensity=1.000000,RayTracingAORadius=200.000000,ColorGradingIntensity=1.000000,ColorGradingLUT=None,DepthOfFieldSensorWidth=24.576000,DepthOfFieldSqueezeFactor=1.000000,DepthOfFieldFocalDistance=0.000000,DepthOfFieldDepthBlurAmount=1.000000,DepthOfFieldDepthBlurRadius=0.000000,DepthOfFieldUseHairDepth=False,DepthOfFieldFocalRegion=0.000000,DepthOfFieldNearTransitionRegion=300.000000,DepthOfFieldFarTransitionRegion=500.000000,DepthOfFieldScale=0.000000,DepthOfFieldNearBlurSize=15.000000,DepthOfFieldFarBlurSize=15.000000,DepthOfFieldOcclusion=0.400000,DepthOfFieldSkyFocusDistance=0.000000,DepthOfFieldVignetteSize=200.000000,MotionBlurAmount=0.500000,MotionBlurMax=5.000000,MotionBlurTargetFPS=30,MotionBlurPerObjectSize=0.000000,TranslucencyType=Raster,RayTracingTranslucencyMaxRoughness=0.600000,RayTracingTranslucencyRefractionRays=3,RayTracingTranslucencySamplesPerPixel=1,RayTracingTranslucencyShadows=Hard_shadows,RayTracingTranslucencyRefraction=True,PathTracingMaxBounces=32,PathTracingSamplesPerPixel=2048,PathTracingMaxPathIntensity=24.000000,PathTracingEnableEmissiveMaterials=True,PathTracingEnableReferenceDOF=False,PathTracingEnableReferenceAtmosphere=False,PathTracingEnableDenoiser=True,PathTracingIncludeEmissive=True,PathTracingIncludeDiffuse=True,PathTracingIncludeIndirectDiffuse=True,PathTracingIncludeSpecular=True,PathTracingIncludeIndirectSpecular=True,PathTracingIncludeVolume=True,PathTracingIncludeIndirectVolume=True,UserFlags=0,WeightedBlendables=(Array=)),LightingRigRotation=0.000000,RotationSpeed=2.000000,DirectionalLightRotation=(Pitch=-40.000000,Yaw=-67.500000,Roll=0.000000),bEnableToneMapping=False,bShowMeshEdges=True)
```

## DefaultEngine.ini

[打开源配置](../../../Config/DefaultEngine.ini)

```ini
   3: [/Script/EngineSettings.GameMapsSettings]
   4: GameDefaultMap=/Game/ThirdPerson/Maps/ThirdPersonMap.ThirdPersonMap
   5: EditorStartupMap=/Game/CodexText/L_MainMenu.L_MainMenu
   6: GlobalDefaultGameMode=/Script/Hodgepodge.HodgepodgeGameModeBase
   7: GameInstanceClass=/Script/Hodgepodge.HodgeGameInstanceBase
   9: [/Script/Engine.Engine]
  10: LocalPlayerClassName=/Script/Hodgepodge.HodgeLocalPlayerBase
  12: [/Script/Engine.RendererSettings]
  13: r.AllowStaticLighting=False
  15: r.GenerateMeshDistanceFields=True
  17: r.DynamicGlobalIlluminationMethod=1
  19: r.ReflectionMethod=1
  21: r.SkinCache.CompileShaders=True
  23: r.RayTracing=True
  25: r.Shadow.Virtual.Enable=1
  27: r.DefaultFeature.AutoExposure.ExtendDefaultLuminanceRange=True
  29: r.DefaultFeature.LocalExposure.HighlightContrastScale=0.8
  31: r.DefaultFeature.LocalExposure.ShadowContrastScale=0.8
  33: [/Script/WindowsTargetPlatform.WindowsTargetSettings]
  34: DefaultGraphicsRHI=DefaultGraphicsRHI_DX12
  35: DefaultGraphicsRHI=DefaultGraphicsRHI_DX12
  36: -D3D12TargetedShaderFormats=PCD3D_SM5
  37: +D3D12TargetedShaderFormats=PCD3D_SM6
  38: -D3D11TargetedShaderFormats=PCD3D_SM5
  39: +D3D11TargetedShaderFormats=PCD3D_SM5
  40: Compiler=Default
  41: AudioSampleRate=48000
  42: AudioCallbackBufferFrameSize=1024
  43: AudioNumBuffersToEnqueue=1
  44: AudioMaxChannels=0
  45: AudioNumSourceWorkers=4
  46: SpatializationPlugin=
  47: SourceDataOverridePlugin=
  48: ReverbPlugin=
  49: OcclusionPlugin=
  50: CompressionOverrides=(bOverrideCompressionTimes=False,DurationThreshold=5.000000,MaxNumRandomBranches=0,SoundCueQualityIndex=0)
  51: CacheSizeKB=65536
  52: MaxChunkSizeOverrideKB=0
  53: bResampleForDevice=False
  54: MaxSampleRate=48000.000000
  55: HighSampleRate=32000.000000
  56: MedSampleRate=24000.000000
  57: LowSampleRate=12000.000000
  58: MinSampleRate=8000.000000
  59: CompressionQualityModifier=1.000000
  60: AutoStreamingThreshold=0.000000
  61: SoundCueCookQualityIndex=-1
  63: [/Script/LinuxTargetPlatform.LinuxTargetSettings]
  64: -TargetedRHIs=SF_VULKAN_SM5
  65: +TargetedRHIs=SF_VULKAN_SM6
  67: [/Script/HardwareTargeting.HardwareTargetingSettings]
  68: TargetedHardwareClass=Desktop
  69: AppliedTargetedHardwareClass=Desktop
  70: DefaultGraphicsPerformance=Maximum
  71: AppliedDefaultGraphicsPerformance=Maximum
  73: [/Script/WorldPartitionEditor.WorldPartitionEditorSettings]
  74: CommandletClass=Class'/Script/UnrealEd.WorldPartitionConvertCommandlet'
  76: [/Script/Engine.UserInterfaceSettings]
  77: bAuthorizeAutomaticWidgetVariableCreation=False
  78: FontDPIPreset=Standard
  79: FontDPI=72
  81: [/Script/Engine.Engine]
  82: +ActiveGameNameRedirects=(OldGameName="TP_BlankBP",NewGameName="/Script/Hodgepodge")
  83: +ActiveGameNameRedirects=(OldGameName="/Script/TP_BlankBP",NewGameName="/Script/Hodgepodge")
  84: AssetManagerClassName=/Script/Hodgepodge.HodgeAssetManager
  86: [/Script/AndroidFileServerEditor.AndroidFileServerRuntimeSettings]
  87: bEnablePlugin=True
  88: bAllowNetworkConnection=True
  89: SecurityToken=[REDACTED]
  90: bIncludeInShipping=False
  91: bAllowExternalStartInShipping=False
  92: bCompileAFSProject=False
  93: bUseCompression=False
  94: bLogFiles=False
  95: bReportStats=False
  96: ConnectionType=USBOnly
  97: bUseManualIPAddress=False
  98: ManualIPAddress=
 100: [/Script/Engine.GarbageCollectionSettings]
 101: gc.DumpObjectCountsToLogWhenMaxObjectLimitExceeded=True
 104: [CoreRedirects]
 105: +ClassRedirects=(OldName="/Script/Hodgepodge.HodgepodgePlayerStateBase",NewName="/Script/Hodgepodge.HodgePlayerStateBase")
 106: +ClassRedirects=(OldName="/Script/Hodgepodge.HodgepodgePlayerControllerBase",NewName="/Script/Hodgepodge.HodgePlayerControllerBase")
 107: +ClassRedirects=(OldName="/Script/Hodgepodge.HodgepodgeGameStateBase",NewName="/Script/Hodgepodge.HodgeGameStateBase")
 108: +ClassRedirects=(OldName="/Script/Hodgepodge.HodgepodgeGameModeBase",NewName="/Script/Hodgepodge.HodgeGameModeBase")
 109: +ClassRedirects=(OldName="/Script/Hodgepodge.HodgepodgeGameInstanceBase",NewName="/Script/Hodgepodge.HodgeGameInstanceBase")
 110: +ClassRedirects=(OldName="/Script/Hodgepodge.HodgepodgeActorComponentBase",NewName="/Script/Hodgepodge.HodgeActorComponentBase")
 111: +ClassRedirects=(OldName="/Script/Hodgepodge.HodgepodgeCombatComponentBase",NewName="/Script/Hodgepodge.HodgeCombatComponentBase")
 112: +ClassRedirects=(OldName="/Script/Hodgepodge.HodgepodgeCharacterBase",NewName="/Script/Hodgepodge.HodgeCharacterBase")
 113: +ClassRedirects=(OldName="/Script/Hodgepodge.HodgepodgeActorBase",NewName="/Script/Hodgepodge.HodgeActorBase")
 114: +ClassRedirects=(OldName="/Script/Hodgepodge.HodgepodgeAbilitySystemComponent",NewName="/Script/Hodgepodge.HodgeAbilitySystemComponent")
 115: +ClassRedirects=(OldName="/Script/Hodgepodge.HodgepodgeAttributeSet",NewName="/Script/Hodgepodge.HodgeAttributeSet")
 116: +ClassRedirects=(OldName="/Script/Hodgepodge.HodgepodgeGameplayAbilityBase",NewName="/Script/Hodgepodge.HodgeGameplayAbilityBase")
 117: +ClassRedirects=(OldName="/Script/Hodgepodge.MyHodgeHeroCharacter",NewName="/Script/Hodgepodge.HodgeHeroCharacter")
 118: +StructRedirects=(OldName="/Script/Hodgepodge.LyraPenetrationAvoidanceFeeler",NewName="/Script/Hodgepodge.HodgePenetrationAvoidanceFeeler")
 119: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraGameplayCueManager",NewName="/Script/Hodgepodge.HodgeGameplayCueManager")
 120: +ClassRedirects=(OldName="/Script/Hodgepodge.HodgeAbilitySystemComponentBase",NewName="/Script/Hodgepodge.HodgeAbilitySystemComponent")
 121: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraGlobalAbilitySystem",NewName="/Script/Hodgepodge.HodgeGlobalAbilitySystem")
 122: +StructRedirects=(OldName="/Script/Hodgepodge.LyraGameplayEffectContext",NewName="/Script/Hodgepodge.HodgeGameplayEffectContext")
 123: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraAbilitySet",NewName="/Script/Hodgepodge.HodgeAbilitySet")
 124: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraAbilityCost",NewName="/Script/Hodgepodge.HodgeAbilityCost")
 125: +ClassRedirects=(OldName="/Script/Hodgepodge.HodgeInputComponentBase",NewName="/Script/Hodgepodge.HodgeInputComponent")
 126: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraPlayerMappableKeyProfile",NewName="/Script/Hodgepodge.HodgePlayerMappableKeyProfile")
 127: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraInputUserSettings",NewName="/Script/Hodgepodge.HodgeInputUserSettings")
 128: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraAimSensitivityData",NewName="/Script/Hodgepodge.HodgeAimSensitivityData")
 129: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraHeroComponent",NewName="/Script/Hodgepodge.HodgeHeroComponent")
 130: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraActivatableWidget",NewName="/Script/Hodgepodge.HodgeActivatableWidget")
 131: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraGameViewportClient",NewName="/Script/Hodgepodge.HodgeGameViewportClient")
 132: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraHUD",NewName="/Script/Hodgepodge.HodgeHUD")
 133: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraHUDLayout",NewName="/Script/Hodgepodge.HodgeHUDLayout")
 134: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraJoystickWidget",NewName="/Script/Hodgepodge.HodgeJoystickWidget")
 135: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraSettingScreen",NewName="/Script/Hodgepodge.HodgeSettingScreen")
 136: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraSimulatedInputWidget",NewName="/Script/Hodgepodge.HodgeSimulatedInputWidget")
 137: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraTaggedWidget",NewName="/Script/Hodgepodge.HodgeTaggedWidget")
 138: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraTouchRegion",NewName="/Script/Hodgepodge.HodgeTouchRegion")
 139: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraWeaponUserInterface",NewName="/Script/Hodgepodge.HodgeWeaponUserInterface")
 140: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraReticleWidgetBase",NewName="/Script/Hodgepodge.HodgeReticleWidgetBase")
 141: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraUIMessaging",NewName="/Script/Hodgepodge.HodgeUIMessaging")
 142: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraUIManagerSubsystem",NewName="/Script/Hodgepodge.HodgeUIManagerSubsystem")
 143: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraPerfStatWidgetBase",NewName="/Script/Hodgepodge.HodgePerfStatWidgetBase")
 144: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraPerfStatContainerBase",NewName="/Script/Hodgepodge.HodgePerfStatContainerBase")
 145: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraIndicatorManagerComponent",NewName="/Script/Hodgepodge.HodgeIndicatorManagerComponent")
 146: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraLobbyBackground",NewName="/Script/Hodgepodge.HodgeLobbyBackground")
 147: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraFrontendStateComponent",NewName="/Script/Hodgepodge.HodgeFrontendStateComponent")
 148: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraActionWidget",NewName="/Script/Hodgepodge.HodgeActionWidget")
 149: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraButtonBase",NewName="/Script/Hodgepodge.HodgeButtonBase")
 150: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraLoadingScreenSubsystem",NewName="/Script/Hodgepodge.HodgeLoadingScreenSubsystem")
 151: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraControllerDisconnectedScreen",NewName="/Script/Hodgepodge.HodgeControllerDisconnectedScreen")
 152: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraConfirmationScreen",NewName="/Script/Hodgepodge.HodgeConfirmationScreen")
 153: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraBoundActionButton",NewName="/Script/Hodgepodge.HodgeBoundActionButton")
 154: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraWidgetFactory_Class",NewName="/Script/Hodgepodge.HodgeWidgetFactory_Class")
 155: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraListView",NewName="/Script/Hodgepodge.HodgeListView")
 156: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraTabButtonBase",NewName="/Script/Hodgepodge.HodgeTabButtonBase")
 157: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraTabListWidgetBase",NewName="/Script/Hodgepodge.HodgeTabListWidgetBase")
 158: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraWidgetFactory",NewName="/Script/Hodgepodge.HodgeWidgetFactory")
 159: +ClassRedirects=(OldName="/Script/Hodgepodge.HogdeWeaponUserInterface",NewName="/Script/Hodgepodge.HodgeWeaponUserInterface")
 160: +ClassRedirects=(OldName="/Script/Hodgepodge.HogdeButtonBase",NewName="/Script/Hodgepodge.HodgeButtonBase")
 161: +ClassRedirects=(OldName="/Script/Hodgepodge.HogdeActivatableWidget",NewName="/Script/Hodgepodge.HodgeActivatableWidget")
 162: +ClassRedirects=(OldName="/Script/Hodgepodge.HogdeSettingScreen",NewName="/Script/Hodgepodge.HodgeSettingScreen")
 163: +ClassRedirects=(OldName="/Script/Hodgepodge.HogdeTouchRegion",NewName="/Script/Hodgepodge.HodgeTouchRegion")
 164: +ClassRedirects=(OldName="/Script/Hodgepodge.HogdeTaggedWidget",NewName="/Script/Hodgepodge.HodgeTaggedWidget")
 165: +ClassRedirects=(OldName="/Script/Hodgepodge.HogdeJoystickWidget",NewName="/Script/Hodgepodge.HodgeJoystickWidget")
 166: +ClassRedirects=(OldName="/Script/Hodgepodge.HodgfeTaggedWidget",NewName="/Script/Hodgepodge.HodgeTaggedWidget")
 167: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraWeaponInstance",NewName="/Script/Hodgepodge.HodgeWeaponInstance")
 168: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraEquipmentManagerComponent",NewName="/Script/Hodgepodge.HodgeEquipmentManagerComponent")
 169: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraEquipmentInstance",NewName="/Script/Hodgepodge.HodgeEquipmentInstance")
 170: +ClassRedirects=(OldName="/Script/Hodgepodge.LyraEquipmentDefinition",NewName="/Script/Hodgepodge.HodgeEquipmentDefinition")
```

## DefaultGame.ini

[打开源配置](../../../Config/DefaultGame.ini)

```ini
   3: [/Script/EngineSettings.GeneralProjectSettings]
   4: ProjectID=31F2898641FDF050F9711F94847F0DB0
   5: ProjectName=Hodgepodge
   6: CopyrightNotice=111屎山代码来袭
   8: [/Script/HodgePodge.HodgeAssetManager]
   9: HodgeGameDataPath=/Game/Main/Data/DA_Dafult_GameData.DA_Dafult_GameData
  10: DefaultPawnData=/Game/Main/Data/DA_Dafult_PawnData.DA_Dafult_PawnData
  12: ; 使用项目自定义 GAS 全局类，使 GAS 默认分配 FHodgeGameplayEffectContext。
  14: [/Script/GameplayAbilities.AbilitySystemGlobals]
  15: AbilitySystemGlobalsClassName=/Script/Hodgepodge.HodgeAbilitySystemGlobals
  16: GlobalGameplayCueManagerClass=/Script/Hodgepodge.HodgeGameplayCueManager
  17: ; Ability 激活失败原因 Tag，对应 HodgeGameplayTags.cpp 中的 Ability.ActivateFail.* 原生 Tag。
  18: ActivateFailCooldownTag=(TagName="Ability.ActivateFail.Cooldown")
  19: ActivateFailCostTag=(TagName="Ability.ActivateFail.Cost")
  20: ActivateFailNetworkingTag=(TagName="Ability.ActivateFail.Networking")
  21: ActivateFailTagsBlockedTag=(TagName="Ability.ActivateFail.TagsBlocked")
  22: ActivateFailTagsMissingTag=(TagName="Ability.ActivateFail.TagsMissing")
  23: ActivateFailCanActivateAbilityTag=(TagName="Ability.ActivateFail.CanActivateAbility")
  25: ; ShowDebug AbilitySystem 使用 HUD 的调试目标（UE 5.5 中默认值为 False）。
  26: bUseDebugTargetFromHud=True
  28: ; 客户端不预测施加到他人目标上的 GameplayEffect（UE 5.5 默认值为 True，与 Lyra 行为不一致）。
  29: PredictTargetGameplayEffects=False
  31: ; 建立 GameplayCue 内容目录后，再取消下行注释并填写常驻加载路径。
  32: ;+GameplayCueNotifyPaths=/Game/Main/GameplayCues
  34: ; 使用项目自定义 GameFeature Policy。
  36: [/Script/GameFeatures.GameFeaturesSubsystemSettings]
  37: GameFeaturesManagerClassName=/Script/Hodgepodge.HodgeGameFeaturePolicy
  39: [/Script/Engine.AssetManagerSettings]
  40: -PrimaryAssetTypesToScan=(PrimaryAssetType="Map",AssetBaseClass=/Script/Engine.World,bHasBlueprintClasses=False,bIsEditorOnly=True,Directories=((Path="/Game/Maps")),SpecificAssets=,Rules=(Priority=-1,ChunkId=-1,bApplyRecursively=True,CookRule=Unknown))
  41: -PrimaryAssetTypesToScan=(PrimaryAssetType="PrimaryAssetLabel",AssetBaseClass=/Script/Engine.PrimaryAssetLabel,bHasBlueprintClasses=False,bIsEditorOnly=True,Directories=((Path="/Game")),SpecificAssets=,Rules=(Priority=-1,ChunkId=-1,bApplyRecursively=True,CookRule=Unknown))
  42: +PrimaryAssetTypesToScan=(PrimaryAssetType="Map",AssetBaseClass="/Script/Engine.World",bHasBlueprintClasses=False,bIsEditorOnly=True,Directories=((Path="/Game/Maps")),SpecificAssets=,Rules=(Priority=-1,ChunkId=-1,bApplyRecursively=True,CookRule=AlwaysCook))
  43: +PrimaryAssetTypesToScan=(PrimaryAssetType="PrimaryAssetLabel",AssetBaseClass="/Script/Engine.PrimaryAssetLabel",bHasBlueprintClasses=False,bIsEditorOnly=True,Directories=((Path="/Game")),SpecificAssets=,Rules=(Priority=-1,ChunkId=-1,bApplyRecursively=True,CookRule=Unknown))
  44: +PrimaryAssetTypesToScan=(PrimaryAssetType="HodgeGameData",AssetBaseClass="/Script/Hodgepodge.HodgeGameData",bHasBlueprintClasses=False,bIsEditorOnly=False,Directories=,SpecificAssets=("/Game/Main/Data/DA_Dafult_GameData.DA_Dafult_GameData"),Rules=(Priority=-1,ChunkId=-1,bApplyRecursively=True,CookRule=AlwaysCook))
  45: +PrimaryAssetTypesToScan=(PrimaryAssetType="GameFeatureData",AssetBaseClass="/Script/GameFeatures.GameFeatureData",bHasBlueprintClasses=False,bIsEditorOnly=False,Directories=((Path="/Game/Unused")),SpecificAssets=,Rules=(Priority=-1,ChunkId=-1,bApplyRecursively=True,CookRule=AlwaysCook))
  46: +PrimaryAssetTypesToScan=(PrimaryAssetType="HodgeExperienceDefinition",AssetBaseClass="/Script/Hodgepodge.HodgeExperienceDefinition",bHasBlueprintClasses=True,bIsEditorOnly=False,Directories=((Path="/Game/Main/Experiences")),SpecificAssets=,Rules=(Priority=-1,ChunkId=-1,bApplyRecursively=True,CookRule=AlwaysCook))
  47: +PrimaryAssetTypesToScan=(PrimaryAssetType="HodgePawnData",AssetBaseClass="/Script/Hodgepodge.HodgePawnData",bHasBlueprintClasses=False,bIsEditorOnly=False,Directories=((Path="/Game/Main/Data")),SpecificAssets=,Rules=(Priority=-1,ChunkId=-1,bApplyRecursively=True,CookRule=AlwaysCook))
  48: +PrimaryAssetTypesToScan=(PrimaryAssetType="HodgeExperienceActionSet",AssetBaseClass="/Script/Hodgepodge.HodgeExperienceActionSet",bHasBlueprintClasses=True,bIsEditorOnly=False,Directories=((Path="/Game/Main/Experiences")),SpecificAssets=,Rules=(Priority=-1,ChunkId=-1,bApplyRecursively=True,CookRule=AlwaysCook))
  49: bOnlyCookProductionAssets=False
  50: bShouldManagerDetermineTypeAndName=False
  51: bShouldGuessTypeAndNameInEditor=True
  52: bShouldAcquireMissingChunksOnLoad=False
  53: bShouldWarnAboutInvalidAssets=True
  54: MetaDataTagsForAssetRegistry=()
  56: [/Script/McpAutomationBridge.McpAutomationBridgeSettings]
  57: bEnableNativeMCP=True
  58: NativeMCPPort=3016
  59: bLoadAllToolsOnStart=True
  60: ListenPorts=8116
  61: bMultiListen=False
  62: bAllowNonLoopback=False
  63: bRequireCapabilityToken=[REDACTED]
```

## DefaultGameplayTags.ini

[打开源配置](../../../Config/DefaultGameplayTags.ini)

```ini
   1: ; =============================================================================
   2: ; Hodgepodge GameplayTag 注册表
   3: ; -----------------------------------------------------------------------------
   4: ; 由 LyraStarterGame (UE 5.5) 的 GameplayTag 全量对照表迁移而来，共 286 条。
   5: ; 迁移规则：原 Lyra.* 命名空间整体重命名为 Hodge.*，其余标签保持原名。
   6: ; 说明：
   7: ;   1. 本文件只登记非原生标签；由 UE_DEFINE_GAMEPLAY_TAG 注册的标签见
   8: ;      Source/Hodgepodge/Private/AbilitySystem/HodgeGameplayTags.cpp
   9: ;      （另有 HodgeHealthSet.cpp、HodgeGameplayAbility.cpp 等处的局部注册）。
  10: ;   2. 原表中的 DataTable 标签（DT_AnimEffectTags / DT_SurfaceTypes）已展开到下方，
  11: ;      故 GameplayTagTableList 暂作注释；日后拷贝同名 DataTable 再取消注释。
  12: ;   3. Explorer.* / TopDownArena.* / Subtitle.* 等来自尚未移植的 Lyra 插件，
  13: ;      此处只登记标签，不代表对应系统已经实现。
  14: ; =============================================================================
  16: [/Script/GameplayTags.GameplayTagsSettings]
  17: ImportTagsFromConfig=True
  18: WarnOnInvalidTags=True
  19: ClearInvalidTags=False
  20: AllowEditorTagUnloading=True
  21: AllowGameTagUnloading=False
  22: FastReplication=False
  23: bDynamicReplication=False
  24: InvalidTagCharacters="\"\',"
  25: NumBitsForContainerSize=6
  26: NetIndexFirstBitSegment=16
  27: ;+GameplayTagTableList=/Game/ContextEffects/DT_AnimEffectTags.DT_AnimEffectTags
  28: ;+GameplayTagTableList=/Game/ContextEffects/DT_SurfaceTypes.DT_SurfaceTypes
  30: ; -----------------------------------------------------------------------------
  31: ; 工程原有标签（保持原样）
  32: ; -----------------------------------------------------------------------------
  33: +GameplayTagList=(Tag="a",DevComment="")
  34: +GameplayTagList=(Tag="Ability.Attack",DevComment="")
  35: ; -----------------------------------------------------------------------------
  36: ; Ability.*
  37: ; -----------------------------------------------------------------------------
  38: +GameplayTagList=(Tag="Ability.ActivateFail.CellAlreadyContainsBomb",DevComment="")
  39: +GameplayTagList=(Tag="Ability.ActivateFail.MagazineFull",DevComment="")
  40: +GameplayTagList=(Tag="Ability.ActivateFail.NoSpareAmmo",DevComment="")
  42: ; -----------------------------------------------------------------------------
  43: ; AnimEffect.*  (来自 DT_AnimEffectTags)
  44: ; -----------------------------------------------------------------------------
  45: +GameplayTagList=(Tag="AnimEffect.Footstep.Jog",DevComment="")
  46: +GameplayTagList=(Tag="AnimEffect.Footstep.Jump",DevComment="")
  47: +GameplayTagList=(Tag="AnimEffect.Footstep.Land",DevComment="")
  48: +GameplayTagList=(Tag="AnimEffect.Footstep.Walk",DevComment="")
  50: ; -----------------------------------------------------------------------------
  51: ; Event.*  (ShooterCore)
  52: ; -----------------------------------------------------------------------------
  53: +GameplayTagList=(Tag="Event.Movement.ADS",DevComment="")
  54: +GameplayTagList=(Tag="Event.Movement.Dash",DevComment="")
  55: +GameplayTagList=(Tag="Event.Movement.Melee",DevComment="")
  56: +GameplayTagList=(Tag="Event.Movement.Reload",DevComment="")
  57: +GameplayTagList=(Tag="Event.Movement.WeaponFire",DevComment="")
  59: ; -----------------------------------------------------------------------------
  60: ; Explorer.*  (ShooterExplorer)
  61: ; -----------------------------------------------------------------------------
  62: +GameplayTagList=(Tag="Explorer",DevComment="")
  63: +GameplayTagList=(Tag="Explorer.Mood.Angry",DevComment="")
  64: +GameplayTagList=(Tag="Explorer.Mood.Awesome",DevComment="")
  65: +GameplayTagList=(Tag="Explorer.Mood.Grumpy",DevComment="")
  66: +GameplayTagList=(Tag="Explorer.Mood.Happy",DevComment="")
  67: +GameplayTagList=(Tag="Explorer.Mood.None",DevComment="")
  68: +GameplayTagList=(Tag="Explorer.SmartObject.Activity",DevComment="")
  69: +GameplayTagList=(Tag="Explorer.SmartObject.Activity.Actions",DevComment="")
  70: +GameplayTagList=(Tag="Explorer.SmartObject.Activity.Actions.Eat",DevComment="")
  71: +GameplayTagList=(Tag="Explorer.SmartObject.Activity.Actions.Emote",DevComment="")
  72: +GameplayTagList=(Tag="Explorer.SmartObject.Activity.Actions.Sit",DevComment="")
  73: +GameplayTagList=(Tag="Explorer.SmartObject.Activity.Actions.Sleep",DevComment="")
  74: +GameplayTagList=(Tag="Explorer.SmartObject.Activity.Actions.Sleep.AtNight",DevComment="")
  75: +GameplayTagList=(Tag="Explorer.SmartObject.Activity.Actions.Sleep.DayTime",DevComment="")
  76: +GameplayTagList=(Tag="Explorer.SmartObject.Activity.General",DevComment="")
  77: +GameplayTagList=(Tag="Explorer.SmartObject.Activity.General.Default",DevComment="")
  78: +GameplayTagList=(Tag="Explorer.SmartObject.Event",DevComment="")
  79: +GameplayTagList=(Tag="Explorer.SmartObject.Event.Interaction",DevComment="")
  80: +GameplayTagList=(Tag="Explorer.SmartObject.Event.Interaction.Ended",DevComment="")
  81: +GameplayTagList=(Tag="Explorer.SmartObject.Event.Interaction.Required",DevComment="")
  82: +GameplayTagList=(Tag="Explorer.SmartObject.Event.Interaction.Required.ToDisable",DevComment="")
  83: +GameplayTagList=(Tag="Explorer.SmartObject.Event.Interaction.Required.ToEnable",DevComment="")
  84: +GameplayTagList=(Tag="Explorer.SmartObject.Event.Interaction.Scripted",DevComment="")
  85: +GameplayTagList=(Tag="Explorer.SmartObject.Event.Interaction.Scripted.ToDisable",DevComment="")
  86: +GameplayTagList=(Tag="Explorer.SmartObject.Event.Interaction.Scripted.ToEnable",DevComment="")
  87: +GameplayTagList=(Tag="Explorer.SmartObject.Event.Interaction.Started",DevComment="")
  88: +GameplayTagList=(Tag="Explorer.SmartObject.Interact",DevComment="")
  89: +GameplayTagList=(Tag="Explorer.SmartObject.Interact.NPC",DevComment="")
  90: +GameplayTagList=(Tag="Explorer.SmartObject.Interact.Player",DevComment="")
  91: +GameplayTagList=(Tag="Explorer.SmartObject.Slot",DevComment="")
  92: +GameplayTagList=(Tag="Explorer.SmartObject.Slot.Sync",DevComment="")
  93: +GameplayTagList=(Tag="Explorer.SmartObject.Slot.Sync.Action",DevComment="")
  94: +GameplayTagList=(Tag="Explorer.SmartObject.Slot.Sync.Dependent",DevComment="")
  95: +GameplayTagList=(Tag="Explorer.SmartObject.Slot.Sync.Primary",DevComment="")
  96: +GameplayTagList=(Tag="Explorer.SmartObject.Slot.Sync.Ready",DevComment="")
  97: +GameplayTagList=(Tag="Explorer.SmartObject.Slot.Sync.Stop",DevComment="")
  98: +GameplayTagList=(Tag="Explorer.SmartObject.State",DevComment="")
  99: +GameplayTagList=(Tag="Explorer.SmartObject.State.Broken",DevComment="")
 100: +GameplayTagList=(Tag="Explorer.SmartObject.State.Enabled",DevComment="")
 101: +GameplayTagList=(Tag="Explorer.SmartObject.State.Interaction",DevComment="")
 102: +GameplayTagList=(Tag="Explorer.SmartObject.State.Interaction.Required",DevComment="")
 103: +GameplayTagList=(Tag="Explorer.SmartObject.State.Interaction.Required.ToDisable",DevComment="")
 104: +GameplayTagList=(Tag="Explorer.SmartObject.State.Interaction.Required.ToEnable",DevComment="")
 106: ; -----------------------------------------------------------------------------
 107: ; Gameplay.*
 108: ; -----------------------------------------------------------------------------
 109: +GameplayTagList=(Tag="Gameplay.Message.ADS",DevComment="Message to UI, Reticle")
 110: +GameplayTagList=(Tag="Gameplay.Message.Nameplate.Add",DevComment="Register Nameplate Source")
 111: +GameplayTagList=(Tag="Gameplay.Message.Nameplate.Discover",DevComment="Looking for nameplates")
 112: +GameplayTagList=(Tag="Gameplay.Message.Nameplate.Remove",DevComment="Unregister Nameplate Source")
 114: ; -----------------------------------------------------------------------------
 115: ; GameplayCue.*
 116: ; -----------------------------------------------------------------------------
 117: +GameplayTagList=(Tag="GameplayCue.Character.Spawn",DevComment="At spawning of the player in shooter game")
 118: +GameplayTagList=(Tag="GameplayCue.ShooterGame.Interact.Collect",DevComment="")
 119: +GameplayTagList=(Tag="GameplayCue.ShooterGame.Interact.WeaponPickup",DevComment="GCN for weapon pick FX attached to pawn")
 120: +GameplayTagList=(Tag="GameplayCue.ShooterGame.UserMessage.MatchDecided",DevComment="")
 121: +GameplayTagList=(Tag="GameplayCue.ShooterGame.UserMessage.WaitingForPlayers",DevComment="")
 122: +GameplayTagList=(Tag="GameplayCue.TopDownArenaGame.PickupAcquired",DevComment="")
 123: +GameplayTagList=(Tag="GameplayCue.TopDownArenaGame.UserMessage.GameOver",DevComment="")
 124: +GameplayTagList=(Tag="GameplayCue.TopDownArenaGame.UserMessage.GetReady",DevComment="")
 125: +GameplayTagList=(Tag="GameplayCue.TopDownArenaGame.UserMessage.WaitingForPlayers",DevComment="")
 127: ; -----------------------------------------------------------------------------
 128: ; GameplayEvent.*
 129: ; -----------------------------------------------------------------------------
 130: +GameplayTagList=(Tag="GameplayEvent.ReloadDone",DevComment="")
 132: ; -----------------------------------------------------------------------------
 133: ; HUD.*
 134: ; -----------------------------------------------------------------------------
 135: +GameplayTagList=(Tag="HUD.Slot.EliminationFeed",DevComment="")
 136: +GameplayTagList=(Tag="HUD.Slot.Equipment",DevComment="")
 137: +GameplayTagList=(Tag="HUD.Slot.ModeStatus",DevComment="")
 138: +GameplayTagList=(Tag="HUD.Slot.PerfStats.Graph",DevComment="")
 139: +GameplayTagList=(Tag="HUD.Slot.PerfStats.Text",DevComment="")
 140: +GameplayTagList=(Tag="HUD.Slot.Reticle",DevComment="")
 141: +GameplayTagList=(Tag="HUD.Slot.TeamScore",DevComment="")
 142: +GameplayTagList=(Tag="HUD.Slot.TopAccolades",DevComment="")
 144: ; -----------------------------------------------------------------------------
 145: ; InputTag.*
 146: ; -----------------------------------------------------------------------------
 147: +GameplayTagList=(Tag="InputTag.Ability.Emote",DevComment="")
 148: +GameplayTagList=(Tag="InputTag.Ability.Interact",DevComment="")
 149: +GameplayTagList=(Tag="InputTag.Ability.Quickslot.CycleBackward",DevComment="")
 150: +GameplayTagList=(Tag="InputTag.Ability.Quickslot.CycleForward",DevComment="")
 151: +GameplayTagList=(Tag="InputTag.Ability.Quickslot.SelectSlot",DevComment="Used to directly select one of the quickbar slots. Intended to be accompanied with a 0-based slot index.")
 152: +GameplayTagList=(Tag="InputTag.Ability.ShowLeaderboard",DevComment="")
 153: +GameplayTagList=(Tag="InputTag.Ability.ToggleInventory",DevComment="")
 154: +GameplayTagList=(Tag="InputTag.Ability.ToggleMap",DevComment="")
 155: +GameplayTagList=(Tag="InputTag.Ability.ToggleMarkerInWorld",DevComment="")
 157: ; -----------------------------------------------------------------------------
 158: ; Hodge.*
 159: ; -----------------------------------------------------------------------------
 160: +GameplayTagList=(Tag="Hodge.AddNotification.KillFeed",DevComment="SendKillFeedInfo to UI")
 161: +GameplayTagList=(Tag="Hodge.ShooterGame.Accolade.EliminationChain",DevComment="")
 162: +GameplayTagList=(Tag="Hodge.ShooterGame.Accolade.EliminationChain.2x",DevComment="")
 163: +GameplayTagList=(Tag="Hodge.ShooterGame.Accolade.EliminationChain.3x",DevComment="")
 164: +GameplayTagList=(Tag="Hodge.ShooterGame.Accolade.EliminationChain.4x",DevComment="")
 165: +GameplayTagList=(Tag="Hodge.ShooterGame.Accolade.EliminationChain.5x",DevComment="")
 166: +GameplayTagList=(Tag="Hodge.ShooterGame.Accolade.EliminationStreak",DevComment="")
 167: +GameplayTagList=(Tag="Hodge.ShooterGame.Accolade.EliminationStreak.5",DevComment="")
 168: +GameplayTagList=(Tag="Hodge.ShooterGame.Accolade.EliminationStreak.10",DevComment="")
 169: +GameplayTagList=(Tag="Hodge.ShooterGame.Accolade.EliminationStreak.15",DevComment="")
 170: +GameplayTagList=(Tag="Hodge.ShooterGame.Accolade.EliminationStreak.20",DevComment="")
 171: +GameplayTagList=(Tag="Hodge.ShooterGame.TDM.TeamScore",DevComment="")
 172: +GameplayTagList=(Tag="Hodge.ShooterGame.Weapon.MagazineAmmo",DevComment="")
 173: +GameplayTagList=(Tag="Hodge.ShooterGame.Weapon.MagazineSize",DevComment="")
 174: +GameplayTagList=(Tag="Hodge.ShooterGame.Weapon.SpareAmmo",DevComment="")
 176: ; -----------------------------------------------------------------------------
 177: ; ShooterGame.*
 178: ; -----------------------------------------------------------------------------
 179: +GameplayTagList=(Tag="ShooterGame.ControlPoint.Captured.Message",DevComment="Fired when a control point has been captured by a team")
 180: +GameplayTagList=(Tag="ShooterGame.ControlPoint.TeamScore",DevComment="")
 181: +GameplayTagList=(Tag="ShooterGame.ExtensionPoint.AbilityBar",DevComment="")
 182: +GameplayTagList=(Tag="ShooterGame.GamePhase.Playing",DevComment="")
 183: +GameplayTagList=(Tag="ShooterGame.GamePhase.PostGame",DevComment="")
 184: +GameplayTagList=(Tag="ShooterGame.GamePhase.Warmup",DevComment="")
 185: +GameplayTagList=(Tag="ShooterGame.Score.Assists",DevComment="")
 186: +GameplayTagList=(Tag="ShooterGame.Score.ControlPointCapture",DevComment="")
 187: +GameplayTagList=(Tag="ShooterGame.Score.Deaths",DevComment="")
 188: +GameplayTagList=(Tag="ShooterGame.Score.Eliminations",DevComment="")
 190: ; -----------------------------------------------------------------------------
 191: ; Subtitle.*  (GameSubtitles)
 192: ; -----------------------------------------------------------------------------
 193: +GameplayTagList=(Tag="Subtitle.TextColor.White",DevComment="")
 194: +GameplayTagList=(Tag="Subtitle.TextColor.Yellow",DevComment="")
 196: ; -----------------------------------------------------------------------------
 197: ; SurfaceType.*  (来自 DT_SurfaceTypes)
 198: ; -----------------------------------------------------------------------------
 199: +GameplayTagList=(Tag="SurfaceType.Character",DevComment="")
 200: +GameplayTagList=(Tag="SurfaceType.Concrete",DevComment="")
 201: +GameplayTagList=(Tag="SurfaceType.Default",DevComment="")
 202: +GameplayTagList=(Tag="SurfaceType.Glass",DevComment="")
 204: ; -----------------------------------------------------------------------------
 205: ; TopDownArena.*
 206: ; -----------------------------------------------------------------------------
 207: +GameplayTagList=(Tag="TopDownArena.ExtensionPoint.AbilityBar",DevComment="")
 208: +GameplayTagList=(Tag="TopDownArena.ExtensionPoint.Players",DevComment="")
 209: +GameplayTagList=(Tag="TopDownArena.ExtensionPoint.StatsBar",DevComment="")
 211: ; -----------------------------------------------------------------------------
 212: ; TODO.*
 213: ; -----------------------------------------------------------------------------
 214: +GameplayTagList=(Tag="TODO.GameModeDamageImmunity",DevComment="")
 216: +GameplayTagList=(Tag="Combo.Entry",DevComment="Definition combo")
 217: +GameplayTagList=(Tag="InputIntent.Attack.Light",DevComment="Definition combo")
 218: +GameplayTagList=(Tag="Combo.Light.01",DevComment="Definition combo")
 219: +GameplayTagList=(Tag="Combo.Light.02",DevComment="Definition combo")
 220: +GameplayTagList=(Tag="Combo.Light.03",DevComment="Definition combo")
 221: +GameplayTagList=(Tag="Combo.Light.04",DevComment="Definition combo")
 222: +GameplayTagList=(Tag="Combo.Light.05",DevComment="Definition combo")
 223: +GameplayTagList=(Tag="Ability.Attack.Light.01",DevComment="Definition combo")
 224: +GameplayTagList=(Tag="Ability.Attack.Light.02",DevComment="Definition combo")
 225: +GameplayTagList=(Tag="Ability.Attack.Light.03",DevComment="Definition combo")
 226: +GameplayTagList=(Tag="Ability.Attack.Light.04",DevComment="Definition combo")
 227: +GameplayTagList=(Tag="Ability.Attack.Light.05",DevComment="Definition combo")
```

## DefaultInput.ini

[打开源配置](../../../Config/DefaultInput.ini)

```ini
   1: [/Script/Engine.InputSettings]
   2: -AxisConfig=(AxisKeyName="Gamepad_LeftX",AxisProperties=(DeadZone=0.25,Exponent=1.f,Sensitivity=1.f))
   3: -AxisConfig=(AxisKeyName="Gamepad_LeftY",AxisProperties=(DeadZone=0.25,Exponent=1.f,Sensitivity=1.f))
   4: -AxisConfig=(AxisKeyName="Gamepad_RightX",AxisProperties=(DeadZone=0.25,Exponent=1.f,Sensitivity=1.f))
   5: -AxisConfig=(AxisKeyName="Gamepad_RightY",AxisProperties=(DeadZone=0.25,Exponent=1.f,Sensitivity=1.f))
   6: -AxisConfig=(AxisKeyName="MouseX",AxisProperties=(DeadZone=0.f,Exponent=1.f,Sensitivity=0.07f))
   7: -AxisConfig=(AxisKeyName="MouseY",AxisProperties=(DeadZone=0.f,Exponent=1.f,Sensitivity=0.07f))
   8: -AxisConfig=(AxisKeyName="Mouse2D",AxisProperties=(DeadZone=0.f,Exponent=1.f,Sensitivity=0.07f))
   9: +AxisConfig=(AxisKeyName="Gamepad_LeftX",AxisProperties=(DeadZone=0.250000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  10: +AxisConfig=(AxisKeyName="Gamepad_LeftY",AxisProperties=(DeadZone=0.250000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  11: +AxisConfig=(AxisKeyName="Gamepad_RightX",AxisProperties=(DeadZone=0.250000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  12: +AxisConfig=(AxisKeyName="Gamepad_RightY",AxisProperties=(DeadZone=0.250000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  13: +AxisConfig=(AxisKeyName="MouseX",AxisProperties=(DeadZone=0.000000,Sensitivity=0.070000,Exponent=1.000000,bInvert=False))
  14: +AxisConfig=(AxisKeyName="MouseY",AxisProperties=(DeadZone=0.000000,Sensitivity=0.070000,Exponent=1.000000,bInvert=False))
  15: +AxisConfig=(AxisKeyName="Mouse2D",AxisProperties=(DeadZone=0.000000,Sensitivity=0.070000,Exponent=1.000000,bInvert=False))
  16: +AxisConfig=(AxisKeyName="MouseWheelAxis",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  17: +AxisConfig=(AxisKeyName="Gamepad_LeftTriggerAxis",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  18: +AxisConfig=(AxisKeyName="Gamepad_RightTriggerAxis",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  19: +AxisConfig=(AxisKeyName="Gamepad_Special_Left_X",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  20: +AxisConfig=(AxisKeyName="Gamepad_Special_Left_Y",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  21: +AxisConfig=(AxisKeyName="Vive_Left_Trigger_Axis",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  22: +AxisConfig=(AxisKeyName="Vive_Left_Trackpad_X",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  23: +AxisConfig=(AxisKeyName="Vive_Left_Trackpad_Y",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  24: +AxisConfig=(AxisKeyName="Vive_Right_Trigger_Axis",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  25: +AxisConfig=(AxisKeyName="Vive_Right_Trackpad_X",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  26: +AxisConfig=(AxisKeyName="Vive_Right_Trackpad_Y",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  27: +AxisConfig=(AxisKeyName="MixedReality_Left_Trigger_Axis",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  28: +AxisConfig=(AxisKeyName="MixedReality_Left_Thumbstick_X",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  29: +AxisConfig=(AxisKeyName="MixedReality_Left_Thumbstick_Y",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  30: +AxisConfig=(AxisKeyName="MixedReality_Left_Trackpad_X",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  31: +AxisConfig=(AxisKeyName="MixedReality_Left_Trackpad_Y",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  32: +AxisConfig=(AxisKeyName="MixedReality_Right_Trigger_Axis",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  33: +AxisConfig=(AxisKeyName="MixedReality_Right_Thumbstick_X",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  34: +AxisConfig=(AxisKeyName="MixedReality_Right_Thumbstick_Y",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  35: +AxisConfig=(AxisKeyName="MixedReality_Right_Trackpad_X",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  36: +AxisConfig=(AxisKeyName="MixedReality_Right_Trackpad_Y",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  37: +AxisConfig=(AxisKeyName="OculusTouch_Left_Grip_Axis",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  38: +AxisConfig=(AxisKeyName="OculusTouch_Left_Trigger_Axis",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  39: +AxisConfig=(AxisKeyName="OculusTouch_Left_Thumbstick_X",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  40: +AxisConfig=(AxisKeyName="OculusTouch_Left_Thumbstick_Y",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  41: +AxisConfig=(AxisKeyName="OculusTouch_Right_Grip_Axis",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  42: +AxisConfig=(AxisKeyName="OculusTouch_Right_Trigger_Axis",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  43: +AxisConfig=(AxisKeyName="OculusTouch_Right_Thumbstick_X",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  44: +AxisConfig=(AxisKeyName="OculusTouch_Right_Thumbstick_Y",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  45: +AxisConfig=(AxisKeyName="ValveIndex_Left_Grip_Axis",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  46: +AxisConfig=(AxisKeyName="ValveIndex_Left_Grip_Force",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  47: +AxisConfig=(AxisKeyName="ValveIndex_Left_Trigger_Axis",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  48: +AxisConfig=(AxisKeyName="ValveIndex_Left_Thumbstick_X",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  49: +AxisConfig=(AxisKeyName="ValveIndex_Left_Thumbstick_Y",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  50: +AxisConfig=(AxisKeyName="ValveIndex_Left_Trackpad_X",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  51: +AxisConfig=(AxisKeyName="ValveIndex_Left_Trackpad_Y",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  52: +AxisConfig=(AxisKeyName="ValveIndex_Left_Trackpad_Force",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  53: +AxisConfig=(AxisKeyName="ValveIndex_Right_Grip_Axis",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  54: +AxisConfig=(AxisKeyName="ValveIndex_Right_Grip_Force",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  55: +AxisConfig=(AxisKeyName="ValveIndex_Right_Trigger_Axis",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  56: +AxisConfig=(AxisKeyName="ValveIndex_Right_Thumbstick_X",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  57: +AxisConfig=(AxisKeyName="ValveIndex_Right_Thumbstick_Y",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  58: +AxisConfig=(AxisKeyName="ValveIndex_Right_Trackpad_X",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  59: +AxisConfig=(AxisKeyName="ValveIndex_Right_Trackpad_Y",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  60: +AxisConfig=(AxisKeyName="ValveIndex_Right_Trackpad_Force",AxisProperties=(DeadZone=0.000000,Sensitivity=1.000000,Exponent=1.000000,bInvert=False))
  61: bAltEnterTogglesFullscreen=True
  62: bF11TogglesFullscreen=True
  63: bUseMouseForTouch=False
  64: bEnableMouseSmoothing=True
  65: bEnableFOVScaling=True
  66: bCaptureMouseOnLaunch=True
  67: bEnableLegacyInputScales=True
  68: bEnableMotionControls=True
  69: bFilterInputByPlatformUser=False
  70: bEnableInputDeviceSubsystem=True
  71: bShouldFlushPressedKeysOnViewportFocusLost=True
  72: bEnableDynamicComponentInputBinding=True
  73: bAlwaysShowTouchInterface=False
  74: bShowConsoleOnFourFingerTap=True
  75: bEnableGestureRecognizer=False
  76: bUseAutocorrect=False
  77: DefaultViewportMouseCaptureMode=CapturePermanently_IncludingInitialMouseDown
  78: DefaultViewportMouseLockMode=LockOnCapture
  79: FOVScale=0.011110
  80: DoubleClickTime=0.200000
  81: +ActionMappings=(ActionName="Jump",bShift=False,bCtrl=False,bAlt=False,bCmd=False,Key=Gamepad_FaceButton_Bottom)
  82: +ActionMappings=(ActionName="Jump",bShift=False,bCtrl=False,bAlt=False,bCmd=False,Key=SpaceBar)
  83: +AxisMappings=(AxisName="Look Up / Down Gamepad",Scale=1.000000,Key=Gamepad_RightY)
  84: +AxisMappings=(AxisName="Look Up / Down Mouse",Scale=-1.000000,Key=MouseY)
  85: +AxisMappings=(AxisName="Move Forward / Backward",Scale=1.000000,Key=Gamepad_LeftY)
  86: +AxisMappings=(AxisName="Move Forward / Backward",Scale=-1.000000,Key=S)
  87: +AxisMappings=(AxisName="Move Forward / Backward",Scale=1.000000,Key=W)
  88: +AxisMappings=(AxisName="Move Right / Left",Scale=-1.000000,Key=A)
  89: +AxisMappings=(AxisName="Move Right / Left",Scale=1.000000,Key=D)
  90: +AxisMappings=(AxisName="Move Right / Left",Scale=1.000000,Key=Gamepad_LeftX)
  91: +AxisMappings=(AxisName="Turn Right / Left Gamepad",Scale=1.000000,Key=Gamepad_RightX)
  92: +AxisMappings=(AxisName="Turn Right / Left Mouse",Scale=1.000000,Key=MouseX)
  93: DefaultPlayerInputClass=/Script/EnhancedInput.EnhancedPlayerInput
  94: DefaultInputComponentClass=/Script/Hodgepodge.HodgeInputComponent
  95: DefaultTouchInterface=/Engine/MobileResources/HUD/DefaultVirtualJoysticks.DefaultVirtualJoysticks
  96: -ConsoleKeys=Tilde
  97: +ConsoleKeys=Tilde
```
