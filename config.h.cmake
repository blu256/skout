#define VERSION "@VERSION@"

// Defined if you have fvisibility and fvisibility-inlines-hidden support.
#cmakedefine __KDE_HAVE_GCC_VISIBILITY 1

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#cmakedefine WORDS_BIGENDIAN @WORDS_BIGENDIAN@

// Needed for external/dmctl to build
#define KDE_CONFDIR "@TDE_CONFIG_DIR@"

// Option: XComposite support
#cmakedefine COMPOSITE 1

// Option: Kicker applet support
#cmakedefine WITH_KICKER_APPLETS 1