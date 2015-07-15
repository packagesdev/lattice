
#import "RSSSettings.h"

typedef NS_ENUM(NSUInteger, RSSLatticeSet)
{
	RSSLatticeSetCustom=0,
	RSSLatticeSetRandom=1,
	RSSLatticeSetRegular=1027,
	RSSLatticeSetChainmail=1028,
	RSSLatticeSetBrassMesh=1029,
	RSSLatticeSetComputer=1030,
	RSSLatticeSetSlick=1031,
	RSSLatticeSetTasty=1032
};

typedef NS_ENUM(NSUInteger, RSSLatticeMaterialType)
{
	RSSLatticeTextureTypeNone=0,
	RSSLatticeTextureTypeIndustrial=1,
	RSSLatticeTextureTypeCrystal=2,
	RSSLatticeTextureTypeChrome=3,
	RSSLatticeTextureTypeBrass=4,
	RSSLatticeTextureTypeShiny=5,
	RSSLatticeTextureTypeGhostly=6,
	RSSLatticeTextureTypeCircuits=7,
	RSSLatticeTextureTypeDoughnuts=8,
	RSSLatticeTextureTypeRandom=9
};

@interface RSSLatticeSettings : RSSSettings

@property RSSLatticeSet standardSet;

@property NSUInteger numberOfLongitudinalDivisionsForTorus;

@property NSUInteger numberOfLatitudinalDivisionsForTorus;

@property NSUInteger torusThickness;

@property RSSLatticeMaterialType torusMaterial;


@property NSUInteger density;

@property NSUInteger depth;

@property NSUInteger fieldOfView;

@property NSUInteger randomnessOfCameraPath;

@property NSUInteger cameraSpeed;


@property BOOL smoothShading;

@property BOOL depthCue;

@property BOOL widescreen;


- (void)resetSettingsToStandardSet:(RSSLatticeSet)inSet;

@end
