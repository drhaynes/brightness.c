# brightness.c

Set brightness of display and keyboard backlight from the Terminal in OS X.
   
###Building
    gcc -o brightness brightness.c -framework IOKit \
       -framework ApplicationServices -framework CoreFoundation

###Usage
    $ brightness <float value between 0..1>
E.g.

    $ brightness 0.01   # ideal for night-time hacking

###Notes
Can be used to set the brightness of the backilghts well below the minimum
level permitted by the normal function keys. This is especially useful for
e.g. setting the keybaord backlight at night (0.01 is a suggested value).

The brightness of Apple displays seems to be hard-limited to approximately 0.0625 (the minimum value possible using the built-in brightness controls). In practical terms, setting a value less than this is permitted, but will not result in the display getting any dimmer.

The keyboard backlight does not appear to have a similar hard limit.

Works on 64-bit machines only, due to use of IOConnectCallScalarMethod
 
###Inspired by:
* [Io AppleExtras](https://github.com/stevedekorte/io/blob/master/addons/AppleExtras) by Steve Dekorte
* [Experimenting With Light On Apple Notebook Computers](http://www.osxbook.com/book/bonus/chapter10/light/) by Amit Singh
