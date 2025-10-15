	.global Width
	.data
Width:
	.quad 100
	.global Height
	.data
Height:
	.quad 50
	.global Map
	.bss
Map:
	.zero 5000
	.global dir
	.data
dir:
	.quad 0
	.global snake_x
	.bss
snake_x:
	.zero 80000
	.global snake_y
	.bss
snake_y:
	.zero 80000
	.global snake_len
	.data
snake_len:
	.quad 4
	.global exit
	.data
exit:
	.quad 0
	.global grown
	.data
grown:
	.quad 0

