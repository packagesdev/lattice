
#import "RSSLatticeConfigurationWindowController.h"

#import "RSSLatticeSettings.h"

#import "RSSCollectionView.h"

#import "RSSCollectionViewItem.h"

@interface RSSLatticeConfigurationWindowController ()
{
	IBOutlet RSSCollectionView *_settingsCollectionView;
	
	
	IBOutlet NSSlider * _longitudinalDivisionsSlider;
	IBOutlet NSTextField * _longitudinalDivisionsLabel;
	IBOutlet NSSlider * _latitudinalDivisionsSlider;
	IBOutlet NSTextField * _latitudinalDivisionsLabel;
	IBOutlet NSSlider * _torusThicknessSlider;
	IBOutlet NSTextField * _torusThicknessLabel;
	IBOutlet NSPopUpButton * _torusMaterialPopupButton;
	
	
	IBOutlet NSSlider * _sceneDensitySlider;
	IBOutlet NSTextField * _sceneDensityLabel;
	IBOutlet NSSlider * _sceneDepthSlider;
	IBOutlet NSTextField * _sceneDepthLabel;
	
	
	IBOutlet NSSlider * _fieldOfViewSlider;
	IBOutlet NSTextField * _fieldOfViewLabel;
	IBOutlet NSSlider * _randomnessOfCameraPathSlider;
	IBOutlet NSTextField * _randomnessOfCameraPathLabel;
	IBOutlet NSSlider * _cameraSpeedSlider;
	IBOutlet NSTextField * _cameraSpeedLabel;
	
	
	IBOutlet NSButton * _smoothShadingCheckBox;
	IBOutlet NSButton * _depthCueCheckBox;
	
	
	IBOutlet NSButton * _widescreenCheckBox;

	NSNumberFormatter * _numberFormatter;
}

- (void)_selectCollectionViewItemWithTag:(NSInteger)inTag;

- (void)_setAsCustomSet;

- (IBAction)setLongitudinalDivisions:(id)sender;
- (IBAction)setLatitudinalDivisions:(id)sender;
- (IBAction)setTorusThickness:(id)sender;
- (IBAction)setTorusMaterial:(id)sender;

- (IBAction)setSceneDensity:(id)sender;
- (IBAction)seSceneDepth:(id)sender;

- (IBAction)setFieldOfView:(id)sender;
- (IBAction)setRandomnessOfCameraPath:(id)sender;
- (IBAction)setCameraSpeed:(id)sender;

- (IBAction)setSmoothShading:(id)sender;
- (IBAction)setDepthCue:(id)sender;

- (IBAction)setWidescreen:(id)sender;

@end

@implementation RSSLatticeConfigurationWindowController

- (void)dealloc
{
	[[NSDistributedNotificationCenter defaultCenter] removeObserver:self];
}

- (Class)settingsClass
{
	return [RSSLatticeSettings class];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	_numberFormatter=[[NSNumberFormatter alloc] init];
	
	if (_numberFormatter!=nil)
	{
		_numberFormatter.hasThousandSeparators=YES;
		_numberFormatter.localizesFormat=YES;
	}
	
	NSBundle * tBundle=[NSBundle bundleForClass:[self class]];
	
	NSArray * tStandardSettingsArray=@[@{
										   RSSCollectionViewRepresentedObjectThumbnail : @"regular_thumbnail",
										   RSSCollectionViewRepresentedObjectTag : @(RSSLatticeSetRegular),
										   RSSCollectionViewRepresentedObjectName : NSLocalizedStringFromTableInBundle(@"Regular",@"Localized",tBundle,@"")
										   },
									   @{
										   RSSCollectionViewRepresentedObjectThumbnail : @"chainmail_thumbnail",
										   RSSCollectionViewRepresentedObjectTag : @(RSSLatticeSetChainmail),
										   RSSCollectionViewRepresentedObjectName : NSLocalizedStringFromTableInBundle(@"Chainmail",@"Localized",tBundle,@"")
										   },
									   @{
										   RSSCollectionViewRepresentedObjectThumbnail : @"brass_mesh_thumbnail",
										   RSSCollectionViewRepresentedObjectTag : @(RSSLatticeSetBrassMesh),
										   RSSCollectionViewRepresentedObjectName : NSLocalizedStringFromTableInBundle(@"Brass Mesh",@"Localized",tBundle,@"")
										   },
									   @{
										   RSSCollectionViewRepresentedObjectThumbnail : @"computer_thumbnail",
										   RSSCollectionViewRepresentedObjectTag : @(RSSLatticeSetComputer),
										   RSSCollectionViewRepresentedObjectName : NSLocalizedStringFromTableInBundle(@"Computer",@"Localized",tBundle,@"")
										   },
									   @{
										   RSSCollectionViewRepresentedObjectThumbnail : @"slick_thumbnail",
										   RSSCollectionViewRepresentedObjectTag : @(RSSLatticeSetSlick),
										   RSSCollectionViewRepresentedObjectName : NSLocalizedStringFromTableInBundle(@"Slick",@"Localized",tBundle,@"")
										   },
									   @{
										   RSSCollectionViewRepresentedObjectThumbnail : @"tasty_thumbnail",
										   RSSCollectionViewRepresentedObjectTag : @(RSSLatticeSetTasty),
										   RSSCollectionViewRepresentedObjectName : NSLocalizedStringFromTableInBundle(@"Tasty",@"Localized",tBundle,@"")
										   },
									   @{
										   RSSCollectionViewRepresentedObjectThumbnail : @"random_thumbnail",
										   RSSCollectionViewRepresentedObjectTag : @(RSSLatticeSetRandom),
										   RSSCollectionViewRepresentedObjectName : NSLocalizedStringFromTableInBundle(@"Random",@"Localized",tBundle,@"")
										   },
									   @{
										   RSSCollectionViewRepresentedObjectThumbnail : @"custom_thumbnail",
										   RSSCollectionViewRepresentedObjectTag : @(RSSLatticeSetCustom),
										   RSSCollectionViewRepresentedObjectName : NSLocalizedStringFromTableInBundle(@"Custom",@"Localized",tBundle,@"")
										   }];
	
	[_settingsCollectionView setContent:tStandardSettingsArray];
	
	/*[[NSDistributedNotificationCenter defaultCenter] addObserver:self
														selector:@selector(preferredScrollerStyleDidChange:)
															name:NSPreferredScrollerStyleDidChangeNotification
														  object:nil
											  suspensionBehavior:NSNotificationSuspensionBehaviorDeliverImmediately];*/
}

- (void)restoreUI
{
	[super restoreUI];
	
	RSSLatticeSettings * tLatticeSettings=(RSSLatticeSettings *) sceneSettings;
	
	[self _selectCollectionViewItemWithTag:tLatticeSettings.standardSet];
	
	[_longitudinalDivisionsSlider setIntegerValue:tLatticeSettings.numberOfLongitudinalDivisionsForTorus];
	[_longitudinalDivisionsLabel setIntegerValue:tLatticeSettings.numberOfLongitudinalDivisionsForTorus];
	
	[_latitudinalDivisionsSlider setIntegerValue:tLatticeSettings.numberOfLatitudinalDivisionsForTorus];
	[_latitudinalDivisionsLabel setIntegerValue:tLatticeSettings.numberOfLatitudinalDivisionsForTorus];
	
	[_torusThicknessSlider setIntegerValue:tLatticeSettings.torusThickness];
	[_torusThicknessLabel setIntegerValue:tLatticeSettings.torusThickness];
	
	[_torusMaterialPopupButton selectItemWithTag:tLatticeSettings.torusMaterial];
	
	
	[_sceneDensitySlider setIntegerValue:tLatticeSettings.density];
	[_sceneDensityLabel setIntegerValue:tLatticeSettings.density];
	
	[_sceneDepthSlider setIntegerValue:tLatticeSettings.depth];
	[_sceneDepthLabel setIntegerValue:tLatticeSettings.depth];
	
	
	[_fieldOfViewSlider setIntegerValue:tLatticeSettings.fieldOfView];
	[_fieldOfViewLabel setIntegerValue:tLatticeSettings.fieldOfView];
	
	[_randomnessOfCameraPathSlider setIntegerValue:tLatticeSettings.randomnessOfCameraPath];
	[_randomnessOfCameraPathLabel setIntegerValue:tLatticeSettings.randomnessOfCameraPath];
	
	[_cameraSpeedSlider setIntegerValue:tLatticeSettings.cameraSpeed];
	[_cameraSpeedLabel setIntegerValue:tLatticeSettings.cameraSpeed];
	
	
	[_smoothShadingCheckBox setState:(tLatticeSettings.smoothShading==YES) ? NSOnState : NSOffState];
	[_depthCueCheckBox setState:(tLatticeSettings.depthCue==YES) ? NSOnState : NSOffState];
	
	[_widescreenCheckBox setState:(tLatticeSettings.widescreen==YES) ? NSOnState : NSOffState];
}

#pragma mark -

- (void)_selectCollectionViewItemWithTag:(NSInteger)inTag
{
	[_settingsCollectionView.content enumerateObjectsUsingBlock:^(NSDictionary * bDictionary,NSUInteger bIndex,BOOL * bOutStop){
		NSNumber * tNumber=bDictionary[RSSCollectionViewRepresentedObjectTag];
		
		if (tNumber!=nil)
		{
			if (inTag==[tNumber integerValue])
			{
				[_settingsCollectionView RSS_selectItemAtIndex:bIndex];
				
				*bOutStop=YES;
			}
		}
	}];
}

- (void)_setAsCustomSet
{
	RSSLatticeSettings * tLatticeSettings=(RSSLatticeSettings *) sceneSettings;
	
	if (tLatticeSettings.standardSet!=RSSLatticeSetCustom)
	{
		tLatticeSettings.standardSet=RSSLatticeSetCustom;
		
		[self _selectCollectionViewItemWithTag:tLatticeSettings.standardSet];
	}
}

- (IBAction)setLongitudinalDivisions:(id)sender
{
	RSSLatticeSettings * tLatticeSettings=(RSSLatticeSettings *) sceneSettings;
	
	if (tLatticeSettings.numberOfLongitudinalDivisionsForTorus!=[sender integerValue])
	{
		tLatticeSettings.numberOfLongitudinalDivisionsForTorus=[sender integerValue];
		
		[_longitudinalDivisionsLabel setIntegerValue:tLatticeSettings.numberOfLongitudinalDivisionsForTorus];
		
		[self _setAsCustomSet];
	}
}

- (IBAction)setLatitudinalDivisions:(id)sender
{
	RSSLatticeSettings * tLatticeSettings=(RSSLatticeSettings *) sceneSettings;
	
	if (tLatticeSettings.numberOfLatitudinalDivisionsForTorus!=[sender integerValue])
	{
		tLatticeSettings.numberOfLatitudinalDivisionsForTorus=[sender integerValue];
		
		[_latitudinalDivisionsLabel setIntegerValue:tLatticeSettings.numberOfLatitudinalDivisionsForTorus];
		
		[self _setAsCustomSet];
	}
}

- (IBAction)setTorusThickness:(id)sender
{
	RSSLatticeSettings * tLatticeSettings=(RSSLatticeSettings *) sceneSettings;
	
	if (tLatticeSettings.torusThickness!=[sender integerValue])
	{
		tLatticeSettings.torusThickness=[sender integerValue];
		
		[_torusThicknessLabel setIntegerValue:tLatticeSettings.torusThickness];
		
		[self _setAsCustomSet];
	}
}

- (IBAction)setTorusMaterial:(id)sender
{
	RSSLatticeSettings * tLatticeSettings=(RSSLatticeSettings *) sceneSettings;
	
	if (tLatticeSettings.torusMaterial!=[sender selectedTag])
	{
		tLatticeSettings.torusMaterial=[sender selectedTag];
		
		[self _setAsCustomSet];
	}
}

- (IBAction)setSceneDensity:(id)sender
{
	RSSLatticeSettings * tLatticeSettings=(RSSLatticeSettings *) sceneSettings;
	
	if (tLatticeSettings.density!=[sender integerValue])
	{
		tLatticeSettings.density=[sender integerValue];
		
		[_sceneDensityLabel setIntegerValue:tLatticeSettings.density];
		
		[self _setAsCustomSet];
	}
}

- (IBAction)seSceneDepth:(id)sender
{
	RSSLatticeSettings * tLatticeSettings=(RSSLatticeSettings *) sceneSettings;
	
	if (tLatticeSettings.depth!=[sender integerValue])
	{
		tLatticeSettings.depth=[sender integerValue];
		
		[_sceneDepthLabel setIntegerValue:tLatticeSettings.depth];
		
		[self _setAsCustomSet];
	}
}

- (IBAction)setFieldOfView:(id)sender
{
	RSSLatticeSettings * tLatticeSettings=(RSSLatticeSettings *) sceneSettings;
	
	if (tLatticeSettings.fieldOfView!=[sender integerValue])
	{
		tLatticeSettings.fieldOfView=[sender integerValue];
		
		[_fieldOfViewLabel setIntegerValue:tLatticeSettings.fieldOfView];
		
		[self _setAsCustomSet];
	}
}

- (IBAction)setRandomnessOfCameraPath:(id)sender
{
	RSSLatticeSettings * tLatticeSettings=(RSSLatticeSettings *) sceneSettings;
	
	if (tLatticeSettings.randomnessOfCameraPath!=[sender integerValue])
	{
		tLatticeSettings.randomnessOfCameraPath=[sender integerValue];
		
		[_randomnessOfCameraPathLabel setIntegerValue:tLatticeSettings.randomnessOfCameraPath];
		
		[self _setAsCustomSet];
	}
}

- (IBAction)setCameraSpeed:(id)sender
{
	RSSLatticeSettings * tLatticeSettings=(RSSLatticeSettings *) sceneSettings;
	
	if (tLatticeSettings.cameraSpeed!=[sender integerValue])
	{
		tLatticeSettings.cameraSpeed=[sender integerValue];
		
		[_cameraSpeedLabel setIntegerValue:tLatticeSettings.cameraSpeed];
		
		[self _setAsCustomSet];
	}
}

- (IBAction)setSmoothShading:(id)sender
{
	RSSLatticeSettings * tLatticeSettings=(RSSLatticeSettings *) sceneSettings;
	
	if (tLatticeSettings.smoothShading!=([sender state]==NSOnState))
	{
		tLatticeSettings.smoothShading=([sender state]==NSOnState);
		
		[self _setAsCustomSet];
	}
}

- (IBAction)setDepthCue:(id)sender
{
	RSSLatticeSettings * tLatticeSettings=(RSSLatticeSettings *) sceneSettings;
	
	if (tLatticeSettings.depthCue!=([sender state]==NSOnState))
	{
		tLatticeSettings.depthCue=([sender state]==NSOnState);
		
		[self _setAsCustomSet];
	}
}

- (IBAction)setWidescreen:(id)sender
{
	RSSLatticeSettings * tLatticeSettings=(RSSLatticeSettings *) sceneSettings;
	
	if (tLatticeSettings.widescreen!=([sender state]==NSOnState))
	{
		tLatticeSettings.widescreen=([sender state]==NSOnState);
	}
}

#pragma mark -

- (BOOL)RSS_collectionView:(NSCollectionView *)inCollectionView shouldSelectItemAtIndex:(NSInteger)inIndex
{
	RSSCollectionViewItem * tCollectionViewItem=(RSSCollectionViewItem *)[_settingsCollectionView itemAtIndex:inIndex];
	
	if (tCollectionViewItem!=nil)
	{
		NSInteger tTag=tCollectionViewItem.tag;
		
		if (tTag==RSSLatticeSetCustom)
			return NO;
	}
	
	return YES;
}

- (void)RSS_collectionViewSelectionDidChange:(NSNotification *)inNotification
{
	if (inNotification.object==_settingsCollectionView)
	{
		NSIndexSet * tIndexSet=[_settingsCollectionView selectionIndexes];
		NSUInteger tIndex=[tIndexSet firstIndex];
		
		RSSCollectionViewItem * tCollectionViewItem=(RSSCollectionViewItem *)[_settingsCollectionView itemAtIndex:tIndex];
		
		if (tCollectionViewItem!=nil)
		{
			NSInteger tTag=tCollectionViewItem.tag;
			RSSLatticeSettings * tLatticeSettings=(RSSLatticeSettings *) sceneSettings;
			
			if (tLatticeSettings.standardSet!=tTag)
			{
				tLatticeSettings.standardSet=tTag;
				
				if (tTag!=RSSLatticeSetRandom)
				{
					[tLatticeSettings resetSettingsToStandardSet:tTag];
					
					[self restoreUI];
				}
			}
		}
	}
}

@end
