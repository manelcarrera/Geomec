defineTest( add_paths ) {

	CMD0="cd PWD && find . -maxdepth 4 -name '*.EXTENSION' -exec dirname {} \; | sort | uniq"

	#defined( EXTRA_PATHS, var ) {
	#	CMD=$$replace( CMD0, PWD, $$EXTRA_PATHS )
	#} else {
	#	CMD=$$replace( CMD0, PWD, $$(PWD) )
	#}

	CMD=$$replace( CMD0, PWD, $$(PWD) )

	VAR=$$1
	EXT_LIST=$$2

	for( e, EXT_LIST) {
		CMD_=$$replace(CMD, EXTENSION, $$e)
		#message( system : $$CMD_ )
		FOLDERS_=$$system( $$CMD_ )
		for( folder, FOLDERS_) { 
			SPLIT=$$split( folder, / )
			contains( SPLIT, 'old' ){ 
				message( 'old' found so ignore )
				next()
			}
			# FIXME: Implement this in project .pro, with 'EXCLUDE_FOLDER' var with a list of folders
			#qtiocompressor
			contains( SPLIT, 'examples' ){ 
				message( 'examples' found so ignore )
				next()
			}
			#WellScene3D : TestViewExe
			contains( SPLIT, 'TestViewExe' ){ 
				message( 'TestViewExe' found so ignore )
				next()
			}
			item=$$folder/*.$$e
			equals( VAR, 'SOURCES' ): SOURCES += $$item
			equals( VAR, 'HEADERS' ): HEADERS += $$item
		}
	}
	equals( VAR, 'SOURCES' ): export (SOURCES)
	equals( VAR, 'HEADERS' ): export (HEADERS)
}

!defined( DONT_ADD_PATHS ,var ) {
	add_paths( 'SOURCES', cpp c cc )
	add_paths( 'HEADERS', h include )
}

SHOW_MESSAGE=1
#unset( SHOW_MESSAGE )
defined( SHOW_MESSAGE ,var ) {
	message( $${FILENAME}.pro : INCLUDEPATH:$$INCLUDEPATH )
	message( $${FILENAME}.pro : HEADERS:$$HEADERS )
	message( $${FILENAME}.pro : SOURCES:$$SOURCES )
}
