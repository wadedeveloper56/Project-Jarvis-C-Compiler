pick1(E_COMMAND, ".lnk")
pick1(E_MAP, ".map")
pick1(E_LIBRARY, ".lib")
#if defined( __UNIX__ )
pick1(E_OBJECT, ".o")
#else
pick1(E_OBJECT, ".obj")
#endif
pick1(E_LOAD, ".exe")
pick1(E_OVL, ".ovl")
pick1(E_DLL, ".dll")
pick1(E_PROTECT, ".exp")
pick1(E_NLM, ".nlm")   /* netware loadable module  */
pick1(E_LAN, ".lan")   /* LAN driver               */
pick1(E_DSK, ".dsk")   /* disk driver              */
pick1(E_NAM, ".nam")   /* name space module        */
pick1(E_NOV_MSL, ".msl")   /* mirrored server link     */
pick1(E_NOV_HAM, ".ham")   /* host adapter module      */
pick1(E_NOV_CDM, ".cdm")   /* custom device module     */
pick1(E_COM, ".com")
pick1(E_REX, ".rex")
#if defined( __UNIX__ )
pick1(E_QNX, "")
#else
pick1(E_QNX, ".qnx")
#endif
pick1(E_SYM, ".sym")
pick1(E_LBC, ".lbc")
#if defined( __UNIX__ )
pick1(E_ELF, "")
#else
pick1(E_ELF, ".elf")
#endif
pick1(E_ILK, ".ilk")
pick1(E_HEX, ".hex")
pick1(E_BIN, ".bin")
