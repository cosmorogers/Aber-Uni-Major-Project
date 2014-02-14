from SimpleCV import Camera

cam = Camera()

while True:
	img = cam.getImage()
	img = img.binarize()
	img.drawText("Hello World!")
	img.show()
