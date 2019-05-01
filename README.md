# XMPlay-plugin-templates
My own templates for writing xmplay plugins


I am using code::blocks 17.12 along with MinGW gcc 5 something. The standard stuff that came along with codeblocks.

To complile you will only need the XMP-SDK which can be downloaded from http://www.un4seen.com/download.php?xmp-sdk at the time of writing this.
Make sure that the path to where the xmpin.h etc are located, is available to your compilers include path. Or simply copy the header files into your project.

In codeblocks you just start a new project, chose "Dynamic Link Library" from the templates. Remove the default main.c and main.h from the project and replace it with the template from this repository.

Notice that a DSP plugin is what you use to make a "general" plugin. Just supply the XMPDSP_FLAG_NODSP flag to the plugin interface struct.

Useful line to add to your post build steps if you use code blocks (to move the compiled DLL directly to your xmplay dir):

cmd /c copy "$(PROJECT_DIR)$(TARGET_OUTPUT_FILE)" "<path-to-your-xmplay-plugin-folder>\xmp-<your-plugin-name>.dll"

Make sure that you name the DLL-file with the prefix "xmp-" otherwise it might not work.