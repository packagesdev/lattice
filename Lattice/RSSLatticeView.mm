
#import "RSSLatticeView.h"

#import "RSSLatticeConfigurationWindowController.h"

#import "RSSUserDefaults+Constants.h"

#import "RSSLatticeSettings.h"

#include "Lattice.h"

#import <OpenGL/gl.h>

@interface RSSLatticeView ()
{
	BOOL _preview;
	BOOL _mainScreen;
	
	BOOL _OpenGLIncompatibilityDetected;
	
	NSOpenGLView *_openGLView;
	
	scene *_scene;
	
	// Preferences
	
	RSSLatticeConfigurationWindowController *_configurationWindowController;
}

+ (void)_initializeScene:(scene *)inScene withSettings:(RSSLatticeSettings *)inSettings;

@end

@implementation RSSLatticeView

+ (void)_initializeScene:(scene *)inScene withSettings:(RSSLatticeSettings *)inSettings
{
	if (inSettings.standardSet==RSSLatticeSetRandom)
	{
		NSUInteger tRandomSet;
		
		tRandomSet=(NSUInteger) SSRandomFloatBetween(RSSLatticeSetRegular,RSSLatticeSetTasty);
		
		[inSettings resetSettingsToStandardSet:(RSSLatticeSet) tRandomSet];
	}
	
	inScene->dLongitude=(int)inSettings.numberOfLongitudinalDivisionsForTorus;
	inScene->dLatitude=(int)inSettings.numberOfLatitudinalDivisionsForTorus;
	inScene->torusThickness=(int)inSettings.torusThickness;
	
	if (inSettings.torusMaterial==RSSLatticeTextureTypeRandom)
		inScene->torusTexture=(int) SSRandomFloatBetween(RSSLatticeSetRegular,RSSLatticeSetTasty);
	else
		inScene->torusTexture=(int)inSettings.torusMaterial;
	
	inScene->density=(int)inSettings.density;
	inScene->dDepth=(int)inSettings.depth;
	
	inScene->dFov=(int)inSettings.fieldOfView;
	inScene->dPathrand=(int)inSettings.randomnessOfCameraPath;
	inScene->dSpeed=(int)inSettings.cameraSpeed;
	
	inScene->dSmooth=(inSettings.smoothShading==YES);
	inScene->dFog=(inSettings.depthCue==YES);
	
	inScene->dWidescreen=(inSettings.widescreen==YES);
}

- (instancetype)initWithFrame:(NSRect)frameRect isPreview:(BOOL)isPreview
{
	self=[super initWithFrame:frameRect isPreview:isPreview];
	
	if (self!=nil)
	{
		_preview=isPreview;
		
		if (_preview==YES)
			_mainScreen=YES;
		else
			_mainScreen= (NSMinX(frameRect)==0 && NSMinY(frameRect)==0);
		
		[self setAnimationTimeInterval:0.05];
	}
	
	return self;
}

- (void)dealloc
{
	if (_scene!=NULL)
	{
		delete _scene;
		_scene=NULL;
	}
}

#pragma mark -

- (void) setFrameSize:(NSSize)newSize
{
	[super setFrameSize:newSize];
	
	if (_openGLView!=nil)
		[_openGLView setFrameSize:newSize];
}

#pragma mark -

- (void) drawRect:(NSRect) inFrame
{
	[[NSColor blackColor] set];
	
	NSRectFill(inFrame);
	
	if (_OpenGLIncompatibilityDetected==YES)
	{
		BOOL tShowErrorMessage=_mainScreen;
		
		if (tShowErrorMessage==NO)
		{
			NSString *tIdentifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
			ScreenSaverDefaults *tDefaults = [ScreenSaverDefaults defaultsForModuleWithName:tIdentifier];
			
			tShowErrorMessage=![tDefaults boolForKey:RSSUserDefaultsMainDisplayOnly];
		}
		
		if (tShowErrorMessage==YES)
		{
			NSRect tFrame=[self frame];
			
			NSMutableParagraphStyle * tMutableParagraphStyle=[[NSParagraphStyle defaultParagraphStyle] mutableCopy];
			[tMutableParagraphStyle setAlignment:NSCenterTextAlignment];
			
			NSDictionary * tAttributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont systemFontOfSize:[NSFont systemFontSize]],NSFontAttributeName,
										  [NSColor whiteColor],NSForegroundColorAttributeName,
										  tMutableParagraphStyle,NSParagraphStyleAttributeName,nil];
			
			
			NSString * tString=NSLocalizedStringFromTableInBundle(@"Minimum OpenGL requirements\rfor this Screen Effect\rnot available\ron your graphic card.",@"Localizable",[NSBundle bundleForClass:[self class]],@"No comment");
			
			NSRect tStringFrame;
			
			tStringFrame.origin=NSZeroPoint;
			tStringFrame.size=[tString sizeWithAttributes:tAttributes];
			
			tStringFrame=SSCenteredRectInRect(tStringFrame,tFrame);
			
			[tString drawInRect:tStringFrame withAttributes:tAttributes];
		}
	}
}

#pragma mark -

- (void)startAnimation
{
	_OpenGLIncompatibilityDetected=NO;
	
	[super startAnimation];
	
	NSString *tIdentifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
	ScreenSaverDefaults *tDefaults = [ScreenSaverDefaults defaultsForModuleWithName:tIdentifier];
	
	BOOL tBool=[tDefaults boolForKey:RSSUserDefaultsMainDisplayOnly];
	
	if (tBool==YES && _mainScreen==NO)
		return;
	
	// Add OpenGLView
	
	NSOpenGLPixelFormatAttribute attribs[] =
	{
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize,(NSOpenGLPixelFormatAttribute)16,
		NSOpenGLPFAMinimumPolicy,
		(NSOpenGLPixelFormatAttribute)0
	};
	
	NSOpenGLPixelFormat *tFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
	
	if (tFormat==nil)
	{
		_OpenGLIncompatibilityDetected=YES;
		return;
	}
	_openGLView = [[NSOpenGLView alloc] initWithFrame:[self bounds] pixelFormat:tFormat];
	
	if (_openGLView!=nil)
	{
		[_openGLView setWantsBestResolutionOpenGLSurface:YES];
		
		[self addSubview:_openGLView];
	}
	else
	{
		_OpenGLIncompatibilityDetected=YES;
		return;
	}
	
	[self lockFocus];
	
	[[_openGLView openGLContext] makeCurrentContext];
	
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	[[_openGLView openGLContext] flushBuffer];
	
	NSRect tPixelBounds=[_openGLView convertRectToBacking:[_openGLView bounds]];
	NSSize tSize=tPixelBounds.size;
	
	_scene=new scene();
	
	if (_scene!=NULL)
	{
		[RSSLatticeView _initializeScene:_scene
							   withSettings:[[RSSLatticeSettings alloc] initWithDictionaryRepresentation:[tDefaults dictionaryRepresentation]]];
		
		_scene->resize((int)tSize.width, (int)tSize.height);
		_scene->create();
	}
	
	const GLint tSwapInterval=1;
	CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval,&tSwapInterval);
	
	[self unlockFocus];
}

- (void)stopAnimation
{
	[super stopAnimation];
	
	if (_scene!=NULL)
	{
		delete _scene;
		_scene=NULL;
	}
}

- (void)animateOneFrame
{
	if (_openGLView!=nil)
	{
		[[_openGLView openGLContext] makeCurrentContext];
		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		if (_scene!=NULL)
			_scene->draw();
		
		glFinish();
		
		[[_openGLView openGLContext] flushBuffer];
	}
}

#pragma mark - Configuration

- (BOOL)hasConfigureSheet
{
	return YES;
}

- (NSWindow*)configureSheet
{
	if (_configurationWindowController==nil)
		_configurationWindowController=[[RSSLatticeConfigurationWindowController alloc] init];
	
	NSWindow * tWindow=_configurationWindowController.window;
	
	[_configurationWindowController restoreUI];
	
	return tWindow;
}

@end
