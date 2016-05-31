# Pixel C debug bridge, rev2

Debugging device for use with Pixel C. Supports the following configurations:

 * Pixel C as host: Configures the Pixel C to act as a USB host, then breaks out two host
   ports via a hub. Useful for demos, for connecting keyboard and mouse, etc.

 * Pixel C as debug device: Configures the Pixel C in debug mode, and joins the main
   device port and the debug port via a second hub to a single convenient micro-B
   debug connector.

Revision 2 is a complete reimplementation, and is still untested at this point.
