src = Split('''
   list_test.c 
''')

component = aos_component('list_test', src)

component.add_cflags('-Wall')
component.add_cflags('-Werror')
