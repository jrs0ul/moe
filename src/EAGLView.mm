#import "EAGLView.h"
#import <CommonCrypto/CommonDigest.h>

@implementation EAGLView

@synthesize animating;
@synthesize initialDistance;
@dynamic animationFrameInterval;



// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}
//------------------------------------
//The EAGL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder
{    
    if ((self = [super initWithCoder:coder]))
    {
        
        _textField = [[UITextField alloc] initWithFrame:CGRectMake(160, 200, 160, 30)];
        [_textField setDelegate:self];
        [_textField setBackgroundColor:[UIColor colorWithWhite:0.0 alpha:0.5]];
        _textField.keyboardType = UIKeyboardTypeASCIICapable;
        _textField.transform = CGAffineTransformMakeRotation(M_PI * (90.0 / 180.0));
        [_textField setTextColor:[UIColor whiteColor]];
        [_textField setFont:[UIFont fontWithName:@"Arial" size:19]];
        [_textField setPlaceholder:@"NONAME"];
        
        
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;

        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

        
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        
        if (!context || ![EAGLContext setCurrentContext:context])
        {
            [self release];
            return nil;
        }
        
      
        
        strncpy(cppstuff.DocumentPath, [[NSSearchPathForDirectoriesInDomains( NSDocumentDirectory
                                              , NSUserDomainMask
                                              , YES) objectAtIndex:0] cString], 255);
        //puts(cppstuff.DocumentPath);
        
        // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
        glGenFramebuffersOES(1, &defaultFramebuffer);
        glGenRenderbuffersOES(1, &colorRenderbuffer);
        glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, colorRenderbuffer);   
        
        animating = FALSE;
        displayLinkSupported = FALSE;
        animationFrameInterval = 1;
        displayLink = nil;
        animationTimer = nil;
        
        //---
        cppstuff.init();
        mus = [IOSMusic alloc];
        [mus open:@"data/music.m4a"];
        
        self.multipleTouchEnabled = YES;
        
        [[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / 60)];
        [[UIAccelerometer sharedAccelerometer] setDelegate:self];
        

        NSString *reqSysVer = @"3.1";
        NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
        if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
            displayLinkSupported = TRUE;
    }

    return self;
}

//-------------------------------------------------
// Saves the user name and score after the user enters it in the provied text field. 
- (void)textFieldDidEndEditing:(UITextField*)textField {
	//Save name
    strcpy(cppstuff.nameToEnter, [[textField text] cString]);
	//[[NSUserDefaults standardUserDefaults] setObject:[textField text] forKey:@"BYBYS"];
	
	//Save the score
	[_textField endEditing:YES];
	[_textField removeFromSuperview];
    cppstuff.TextEntered = true;
}

// Terminates the editing session
- (BOOL)textFieldShouldReturn:(UITextField*)textField {
	//Terminate editing
	[textField resignFirstResponder];
	
	return YES;
}
//-------------------------------
NSString * md5( NSString *str ){
	const char *cStr = [str UTF8String];
    
	unsigned char result[CC_MD5_DIGEST_LENGTH];
    
	CC_MD5( cStr, strlen(cStr), result );
    
	return [NSString 
			stringWithFormat: @"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
			result[0], result[1], result[2], result[3],
			result[4], result[5], result[6], result[7], result[8], result[9],
			result[10], result[11], result[12], result[13], result[14], result[15]
			];
    
}
//-------------------
- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    //puts("Pooom");
    cppstuff.getHighScore = true;
    cppstuff.gotHighScore = false;
    cppstuff.failToGetScore = false;
    cppstuff.waitForOnlineScoreTics = 0;
    strcpy(cppstuff.highScoreFromServer,"");
    
}
//--------------------
- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error{
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"No connection?" message:@"Impossible to send your score" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
    [alert show];
    [alert release];
    
}

//--------------------------------
-(void) sendScore{
    //NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    NSString *urlToAuthPage = [[NSString alloc] initWithCString:cppstuff.highscorePostParams];
    urlToAuthPage = [urlToAuthPage stringByAppendingString:md5(@"pikti_satanistai")];
    //NSLog(urlToAuthPage);
    
    NSData *postData = [urlToAuthPage dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:NO];
    NSString *postLength = [NSString stringWithFormat:@"%d",[urlToAuthPage length]];
    NSMutableURLRequest *request = [[[NSMutableURLRequest alloc] init] autorelease];
    NSString* tmpurl = [NSString stringWithFormat:@"http://jrs0ul.com/score/BumpyDroids.php?mode=add&type="];
    tmpurl = [tmpurl stringByAppendingString:[NSString stringWithFormat:@"%d",cppstuff.onlineScoreType]];
    [request setURL:[NSURL URLWithString:tmpurl]];
    [request setHTTPMethod:@"POST"];
    [request setValue:postLength forHTTPHeaderField:@"Content-Length"];
    [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
    [request setHTTPBody:postData];
    NSURLConnection *conn = [[NSURLConnection alloc]initWithRequest:request delegate:self];
    if (conn) {
        NSLog(@"Connection Successful");
    } 
    else {
        /*UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Error Connection" message:@"There was an error connecting" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
        [alert show];
        [alert release];*/
    }
    //[tmpurl release];
    //[request release];
    //[urlToAuthPage release];
}
//--------------------------------
-(void) getScore{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    NSString *myURL = [[NSString alloc] initWithFormat:@"http://jrs0ul.com/score/BumpyDroids.php?mode=get&type=%d", cppstuff.onlineScoreType];
    
    NSString *result = [[NSString alloc] initWithContentsOfURL:[NSURL URLWithString:myURL]];
    
    if ([result length] > 0){
        strncpy(cppstuff.highScoreFromServer, [result cString], 2024);
        cppstuff.gotHighScore = true; 
    }
    //NSLog(@"Response from server: %@",result);
    [myURL release];
    [result release];
    [pool release];
    [NSThread exit];
}

//--------------------------------------------------------------
- (void)drawView:(id)sender{
       
    
    [EAGLContext setCurrentContext:context];
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, defaultFramebuffer);
    glViewport(0, 0, backingWidth, backingHeight);
    cppstuff.render();
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    [context presentRenderbuffer:GL_RENDERBUFFER_OES];
    //------------------------------
    cppstuff.logic();
    
    if (cppstuff.ChangeVolume){
        [mus setVolume:cppstuff.MusicVolume];
        cppstuff.ChangeVolume = false;
    }
    
    if (cppstuff.PlayNewSong){
        cppstuff.PlayNewSong = false;
        [mus dealloc];
        mus = [IOSMusic alloc];
        [mus open:[NSString stringWithCString:cppstuff.songName]];
        [mus setVolume:cppstuff.MusicVolume];
        [mus play];
    }
    
    if (cppstuff.buyGame){
        NSString *appstoreLink = @"itms-apps://itunes.com/apps/BumpyDroids";
        
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:appstoreLink]];
        cppstuff.buyGame = false;
    }
    
    if (cppstuff.PlayMusic){
        [mus play];
        cppstuff.PlayMusic = false;
    }
    
    if (cppstuff.launchjrs0ul){
        
        NSString *appstoreLink = @"itms-apps://itunes.com/apps/jrs0ul";
        
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:appstoreLink]];
        //[[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"http://jrs0ul.com/en/projects/"]];
        cppstuff.launchjrs0ul = false;
    }
    
    if (cppstuff.ActivateEditField){
        [_textField setText:[NSString stringWithCString:cppstuff.nameToEnter length:strlen(cppstuff.nameToEnter)]];
        [self addSubview:_textField];
        [_textField becomeFirstResponder];
        cppstuff.ActivateEditField = false;
    }
    
    
    if (cppstuff.sendScore){
        /*[NSThread detachNewThreadSelector:@selector(sendScore) 
                                 toTarget:self 
                               withObject:nil];*/
        
        [self sendScore];
        cppstuff.sendScore = false;
    }
    
    if (cppstuff.getHighScore){
        [NSThread detachNewThreadSelector:@selector(getScore) 
                                 toTarget:self 
                               withObject:nil];
        cppstuff.getHighScore = false;
        //[this getScore];
    }
    

}
//--------------------------------------------------
- (void)layoutSubviews{
    
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, colorRenderbuffer);
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer *)self.layer];
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
        NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
    
    [self drawView:nil];
}
//----------------------------------------
- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}
//----------------------------------------
- (void)setAnimationFrameInterval:(NSInteger)frameInterval{
      if (frameInterval >= 1)
    {
        animationFrameInterval = frameInterval;

        if (animating)
        {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}
//------------------------------------
- (void)startAnimation{
    if (!animating){
        
        if (displayLinkSupported){
            displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
            [displayLink setFrameInterval:animationFrameInterval];
            [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        }
        else
            animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(drawView:) userInfo:nil repeats:TRUE];

        animating = TRUE;
        
        //cppstuff.music.playback();
        [mus play];
    }
}
//----------------------------------------
- (void)stopAnimation
{
    if (animating)
    {
        if (displayLinkSupported)
        {
            [displayLink invalidate];
            displayLink = nil;
        }
        else
        {
            [animationTimer invalidate];
            animationTimer = nil;
        }

        animating = FALSE;
        cppstuff.gamestate = TITLE;
        //cppstuff.music.stop();
        [mus stop];
    }
}
//---------------------------------------
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
       
    cppstuff.touches.down.destroy();
    if ([touches count])
        cppstuff.touches.allfingersup = false;
    for (unsigned i = 0; i < [touches count]; i++){
        UITouch *t = [[touches allObjects] objectAtIndex:i];
        Vector3D v([t locationInView:self].y, 
                   320 - [t locationInView:self].x, 0);
        cppstuff.touches.down.add(v);
    }
   }
//------------------------------------
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    cppstuff.touches.move.destroy();
    if ([touches count])
        cppstuff.touches.allfingersup = false;
    for (unsigned i = 0; i < [touches count]; i++){
        UITouch *t = [[touches allObjects] objectAtIndex:i];
        Vector3D v([t locationInView:self].y, 
                   320 - [t locationInView:self].x, 0);
        cppstuff.touches.move.add(v);
    }
}
//------------------------------------

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    cppstuff.touches.up.destroy();
    for (unsigned i = 0; i < [touches count]; i++){
        UITouch *t = [[touches allObjects] objectAtIndex:i];
        Vector3D v([t locationInView:self].y, 
                   320 - [t locationInView:self].x, 0);
        cppstuff.touches.up.add(v);
    }
    //----
    if ([touches count] == [[event touchesForView:self] count]) //all fingers up
        cppstuff.touches.allfingersup = true;
   
    
}
//------------------------------------

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    cppstuff.touches.up.destroy();
    cppstuff.touches.move.destroy();
    cppstuff.touches.down.destroy();
   
    cppstuff.touches.allfingersup = true;
  
}
//------------------------------------
- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration{
    
    float kFilteringFactor = 0.8f;
    
    cppstuff.accelerometer.v[0] = acceleration.x * kFilteringFactor + cppstuff.accelerometer.v[0] * (1.0 - kFilteringFactor);
    cppstuff.accelerometer.v[1] = acceleration.y * kFilteringFactor + cppstuff.accelerometer.v[1] * (1.0 - kFilteringFactor);
    cppstuff.accelerometer.v[2] = acceleration.z * kFilteringFactor + cppstuff.accelerometer.v[2] * (1.0 - kFilteringFactor);
    
    
}

//---------------------------------------
- (void)dealloc{
    puts("Destroy");
    
    [_textField release];
    // Tear down GL
    if (defaultFramebuffer){
        glDeleteFramebuffersOES(1, &defaultFramebuffer);
        defaultFramebuffer = 0;
    }
    
    if (colorRenderbuffer){
        glDeleteRenderbuffersOES(1, &colorRenderbuffer);
        colorRenderbuffer = 0;
    }
    
    cppstuff.destroy();
    [mus dealloc];
    
    // Tear down context
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
    
    [context release];
    context = nil;
    [super dealloc];
}

@end
