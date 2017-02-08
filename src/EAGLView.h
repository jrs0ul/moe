

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import "CppSingleton.h"
#import "audio/iOSMusic.h"



@interface EAGLView : UIView <UIAccelerometerDelegate, UITextFieldDelegate>
{    
@private
    EAGLContext *context;
    GLint backingWidth;
    GLint backingHeight;
    
    IOSMusic* mus;
    
    GLuint defaultFramebuffer, colorRenderbuffer;
    
    BOOL animating;
    BOOL displayLinkSupported;
    NSInteger animationFrameInterval;
    id displayLink;
    NSTimer *animationTimer;
    
    UITextField* _textField;
    //IBOutlet UIWindow		*window;
    Singleton cppstuff;
       
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;
@property CGFloat initialDistance;

//multitouch
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event;
//accel
- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration;


- (void)startAnimation;
- (void)stopAnimation;
- (void)drawView:(id)sender;

-(void)sendScore;
-(void)getScore;

@end
