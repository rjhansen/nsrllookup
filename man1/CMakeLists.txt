if (UNIX)
  set(MAN_NAMES nsrllookup.1)
  add_custom_target(man ALL DEPENDS ${MAN_NAMES})
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/nsrllookup.1 DESTINATION
    ${CMAKE_INSTALL_MANDIR}/man1/)
endif()
