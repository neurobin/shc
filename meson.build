project('shc', 'c')
executable('shc', 'src/shc.c', install : true, install_dir : get_option('bindir'))

install_data(sources: 'shc.1', install_dir : get_option('datadir') / 'man/man1')
install_data(['AUTHORS', 'ChangeLog', 'NEWS', 'README', 'README.md'],
			install_dir : get_option('datadir') / 'shc')
install_data(sources: 'COPYING', install_dir : get_option('datadir') / 'licenses/shc')
