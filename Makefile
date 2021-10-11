test: user UM shsh admin

user: user.c
	gcc -o user.exe user.c
shsh: shsh.c
	gcc -o shsh.exe shsh.c
admin: admin.c
	gcc -o admin.exe admin.c
UM: UM.c
	gcc -pthread -o UM.exe UM.c
	