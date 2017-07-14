
#import "RSSLatticeSettings.h"

NSString * const RSSLattice_Settings_StandardSetKey=@"Standard set";

NSString * const RSSLattice_Settings_TorusLongitudinalDivisionsKey=@"Longitude";
NSString * const RSSLattice_Settings_TorusLatitudinalDivisionsKey=@"Latitude";
NSString * const RSSLattice_Settings_TorusThicknessKey=@"Thick";
NSString * const RSSLattice_Settings_TorusMaterialKey=@"Texture";

NSString * const RSSLattice_Settings_SceneDensityKey=@"Density";
NSString * const RSSLattice_Settings_SceneDepthKey=@"Depth";

NSString * const RSSLattice_Settings_FieldOfViewKey=@"Fov";
NSString * const RSSLattice_Settings_RandomnessOfCameraPathKey=@"Pathrand";
NSString * const RSSLattice_Settings_CameraSpeedKey=@"Speed";

NSString * const RSSLattice_Settings_SmoothShadingKey=@"Smooth";
NSString * const RSSLattice_Settings_DepthCueKey=@"Fog";

NSString * const RSSLattice_Settings_WidescreenKey=@"WideScreen";

@implementation RSSLatticeSettings

- (id)initWithDictionaryRepresentation:(NSDictionary *)inDictionary
{
	self=[super init];
	
	if (self!=nil)
	{
		NSNumber * tNumber=inDictionary[RSSLattice_Settings_StandardSetKey];
		
		if (tNumber==nil)
			_standardSet=RSSLatticeSetRegular;
		else
			_standardSet=[tNumber unsignedIntegerValue];
		
		if (_standardSet==RSSLatticeSetCustom)
		{
			_numberOfLongitudinalDivisionsForTorus=[inDictionary[RSSLattice_Settings_TorusLongitudinalDivisionsKey] unsignedIntegerValue];
			_numberOfLatitudinalDivisionsForTorus=[inDictionary[RSSLattice_Settings_TorusLatitudinalDivisionsKey] unsignedIntegerValue];
			_torusThickness=[inDictionary[RSSLattice_Settings_TorusThicknessKey] unsignedIntegerValue];
			_torusMaterial=[inDictionary[RSSLattice_Settings_TorusMaterialKey] unsignedIntegerValue];
			 
			_density=[inDictionary[RSSLattice_Settings_SceneDensityKey] unsignedIntegerValue];
			_depth=[inDictionary[RSSLattice_Settings_SceneDepthKey] unsignedIntegerValue];
			 
			_fieldOfView=[inDictionary[RSSLattice_Settings_FieldOfViewKey] unsignedIntegerValue];
			_randomnessOfCameraPath=[inDictionary[RSSLattice_Settings_RandomnessOfCameraPathKey] unsignedIntegerValue];
			_cameraSpeed=[inDictionary[RSSLattice_Settings_CameraSpeedKey] unsignedIntegerValue];
			
			_smoothShading=[inDictionary[RSSLattice_Settings_SmoothShadingKey] boolValue];
			_depthCue=[inDictionary[RSSLattice_Settings_DepthCueKey] boolValue];
			
		}
		else if (_standardSet!=RSSLatticeSetRandom)
		{
			[self resetSettingsToStandardSet:_standardSet];
		}
		
		_widescreen=[inDictionary[RSSLattice_Settings_WidescreenKey] boolValue];
	}
	
	return self;
}

- (NSDictionary *)dictionaryRepresentation
{
	NSMutableDictionary * tMutableDictionary=[NSMutableDictionary dictionary];
	
	if (tMutableDictionary!=nil)
	{
		tMutableDictionary[RSSLattice_Settings_StandardSetKey]=@(_standardSet);
		
		tMutableDictionary[RSSLattice_Settings_TorusLongitudinalDivisionsKey]=@(_numberOfLongitudinalDivisionsForTorus);
		tMutableDictionary[RSSLattice_Settings_TorusLatitudinalDivisionsKey]=@(_numberOfLatitudinalDivisionsForTorus);
		tMutableDictionary[RSSLattice_Settings_TorusThicknessKey]=@(_torusThickness);
		tMutableDictionary[RSSLattice_Settings_TorusMaterialKey]=@(_torusMaterial);
		 
		tMutableDictionary[RSSLattice_Settings_SceneDensityKey]=@(_density);
		tMutableDictionary[RSSLattice_Settings_SceneDepthKey]=@(_depth);
		 
		tMutableDictionary[RSSLattice_Settings_FieldOfViewKey]=@(_fieldOfView);
		tMutableDictionary[RSSLattice_Settings_RandomnessOfCameraPathKey]=@(_randomnessOfCameraPath);
		tMutableDictionary[RSSLattice_Settings_CameraSpeedKey]=@(_cameraSpeed);
		 
		tMutableDictionary[RSSLattice_Settings_SmoothShadingKey]=@(_smoothShading);
		tMutableDictionary[RSSLattice_Settings_DepthCueKey]=@(_depthCue);
		 
		tMutableDictionary[RSSLattice_Settings_WidescreenKey]=@(_widescreen);
	}
	
	return [tMutableDictionary copy];
}

#pragma mark -

- (void)resetSettings
{
	_standardSet=RSSLatticeSetRegular;
	
	[self resetSettingsToStandardSet:_standardSet];
}

- (void)resetSettingsToStandardSet:(RSSLatticeSet)inSet;
{
	switch(inSet)
	{
		case RSSLatticeSetRegular:
			
			_numberOfLongitudinalDivisionsForTorus = 16;
			_numberOfLatitudinalDivisionsForTorus = 8;
			_torusThickness = 50;
			_torusMaterial = RSSLatticeTextureTypeNone;
			
			_density = 50;
			_depth = 4;
			
			_fieldOfView = 90;
			_randomnessOfCameraPath = 7;
			_cameraSpeed = 10;

			_smoothShading = NO;
			_depthCue = YES;
			
			break;
			
		case RSSLatticeSetChainmail:
			
			_numberOfLongitudinalDivisionsForTorus = 24;
			_numberOfLatitudinalDivisionsForTorus = 12;
			_torusThickness = 50;
			_torusMaterial = RSSLatticeTextureTypeChrome;
			
			_density = 80;
			_depth = 4;
			
			_fieldOfView = 90;
			_randomnessOfCameraPath = 7;
			_cameraSpeed = 10;
			
			_smoothShading = YES;
			_depthCue = YES;
			
			break;
			
		case RSSLatticeSetBrassMesh:
			
			_numberOfLongitudinalDivisionsForTorus = 4;
			_numberOfLatitudinalDivisionsForTorus = 4;
			_torusThickness = 40;
			_torusMaterial = RSSLatticeTextureTypeBrass;
			
			_density = 50;
			_depth = 4;
			
			_fieldOfView = 90;
			_randomnessOfCameraPath = 7;
			_cameraSpeed = 10;
			
			_smoothShading = NO;
			_depthCue = YES;
			
			break;
			
		case RSSLatticeSetComputer:
			
			_numberOfLongitudinalDivisionsForTorus = 4;
			_numberOfLatitudinalDivisionsForTorus = 6;
			_torusThickness = 70;
			_torusMaterial = RSSLatticeTextureTypeCircuits;
			
			_density = 90;
			_depth = 4;
			
			_fieldOfView = 90;
			_randomnessOfCameraPath = 7;
			_cameraSpeed = 10;
			
			_smoothShading = NO;
			_depthCue = YES;
			
			break;
			
		case RSSLatticeSetSlick:
			
			_numberOfLongitudinalDivisionsForTorus = 24;
			_numberOfLatitudinalDivisionsForTorus = 12;
			_torusThickness = 100;
			_torusMaterial = RSSLatticeTextureTypeShiny;
			
			_density = 30;
			_depth = 4;
			_fieldOfView = 90;
			_randomnessOfCameraPath = 7;
			_cameraSpeed = 10;
			
			_smoothShading = YES;
			_depthCue = YES;
			
			break;
			
		case RSSLatticeSetTasty:
			
			_numberOfLongitudinalDivisionsForTorus = 24;
			_numberOfLatitudinalDivisionsForTorus = 12;
			_torusThickness = 100;
			_torusMaterial = RSSLatticeTextureTypeDoughnuts;
			
			_density = 25;
			_depth = 4;
			
			_fieldOfView = 90;
			_randomnessOfCameraPath = 7;
			_cameraSpeed = 10;
			_smoothShading = YES;
			_depthCue = YES;
			
			break;
			
		default:
			
			NSLog(@"This should not be invoked for set: %u",(unsigned int)inSet);
			
			break;
	}
}

@end

