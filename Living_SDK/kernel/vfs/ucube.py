src     = Split('''
        vfs.c
        select.c
        device.c
        vfs_file.c
        vfs_inode.c
        vfs_register.c
''')
component = aos_component('vfs', src)

if aos_global_config.ide != 'keil':
    component.add_comp_deps('kernel/vfs/device')

component.add_global_includes('include')

component.add_global_macros('AOS_VFS')
