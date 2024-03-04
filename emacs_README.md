 Basic curve in EMACS
  1.  Start from the basic tutorial
   C-h t    C stands for CTRL and M stands for ALT

TAGS in emacs
  TAGS are very much useful in emacs.
  TAGS gives the entire references of the functions in your project.
  TAG tables can be created and can be used as follows
  1. In your project directory run the command
     	 etags $(find . -name \*.[ch]) --declarations
  2. In emacs file run the following
     	First press M-! which will lead to shell command
	Now type
	M-x visit-tags-table
	press Enter
	Now give the TAGS file location
  3. In your project directory you should see the file named as "TAGS"
  4. To check go to any function and press M-.
  5. If successfull it will show the instances of the function.


TAGS for entire C library
 1. Same as the above but this time, the project directory is /usr/include
 2. Follow the above "TAGS in emacs"
 3. If any prompt comes just click y

TAGS for linux header files for device drivers

 1. Same as the above but this time, the project directory is /lib/modules/5.15.0-67-generic/build/include/linux
 2. Follow the above "TAGS in emacs"
 3. If any prompt comes just click y

References

https://ftp.gnu.org/pub/old-gnu/Manuals/emacs-20.7/html_node/emacs_266.html
//upto emacs_272 follow

https://courses.cs.washington.edu/courses/cse451/19wi/tutorials/ctags.html
// see Ctags with Emacs

emacs_refcard.pdf

emacs.pdf