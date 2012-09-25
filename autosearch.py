from scipy import *

# Function printEntireArray
# Input: An array that needs to be printed
# Returns: Nothing
# Side-Effects: Displays the entire array to stdout, using no padding in
# 				between elements
def printEntireArray(array):
	print "Array dimension: ",len(array),len(array[0]);
	for y in range(0,len(array)):
		result = ""
		for x in range(0,len(array[0])):
			result += "" + str(array[y][x]) + ""
		print result

# Function gpsToMeters()
# Input: Displacement (in GPS degrees)
# Returns: Displacement (in meters)
# Side-Effects: None
def gpsToMeters(gps):
	return gps * 111122.0 # TODO build proper algorithm

# Function metersToGps()
# Input: Displacement (in meters)
# Output: Displacement (in GPS)
# Side-Effects: None
def metersToGps(meters):
	return meters / 111122.0 # TODO build proper algorithm
		
# Input: A tuple of coordinates, where each coordinate is an array, with the
#		 0th value b

def getCenterCoord(coords):
	x = 0
	y = 0
	for coord in coords:
		x += coord[0]
		y += coord[1]
	x /= len(coords)
	y /= len(coords)
	return x,y
	
# GPSCoord: Contains a longitude and latitude
class GPSCoord:
	
	# Create a new GPS coordinate from latitude, longitude, and altitude
	def __init__(self,latitude,longitude,altitude):
		self.lon = longitude
		self.lat = latitude
		self.alt = altitude
		
	# Function getMeterCoords()
	# Input: (self)
	# Output: The x, y, and z values of this GPS coordinate
	def getMeterCoords(self):
		return GPStoMeters(lat),GPStoMeters(lon),alt

		
class Point:
	
	def __init__(self,pt):
		self.x = pt[0]
		self.y = pt[1]
		
class AutoSearch:

	BORDER = metersToGps(3) # Border around the map, in meters. so nothing on the edges is missed.
	
	def printSelf(self):
		printEntireArray(self.array);
		
		
	# Function: init()
	# Input: An array of GPS coordinates
	# Output: Nothing
	# Side Effects: Allocates and initializes main array, origin_gps_self.width_meters, and self.height_meters
	def __init__(self,coords):
		self.bottom_bound, self.top_bound, self.left_bound, self.right_bound = self.__getBounds(coords);
		self.width_gps = self.right_bound - self.left_bound
		self.height_gps = self.top_bound - self.bottom_bound
		print "Width (GPS): ", self.width_gps
		print "Height (GPS): ", self.height_gps
		self.width_meters = math.ceil(gpsToMeters(self.width_gps)) 	# Fix this later. There should be a function lonToMeters(lat,gps)
		self.height_meters = math.ceil(gpsToMeters(self.height_gps))	# Fix this later. There should be a function latToMeters(gps)
		print "Width (Meters): ", int(self.width_meters)
		print "Height (Meters): ", int(self.height_meters)
		self.array = ones ( (int(self.width_meters),int(self.height_meters)),dtype('i1'))
		self.__connectLines(coords,0)
		self.__fill(0,0,0);
		printEntireArray(self.array);
	
	def __gpsToArrayCoord(self,lat,lon):
		return gpsToMeters(lat-self.left_bound),gpsToMeters(lon-self.bottom_bound)

	def showImage(self, filename):
		pass
		# print "Saving....",
		# image = System.Drawing.Bitmap(len(self.array),len(self.array[0]))
		# image = Image.new("RGB", (len(self.array),len(self.array[0])))
		# pix = image.load()
		# for row in self.array:
		# 	for element in row:
		# 		if ( element == 0 ):
		# 			pass
		# 			# pix[x,y] = (255,0,0)
		# 			#image.SetPixel(x, y, System.Drawing.Color.Red)
		# 		elif ( element == 1):
		# 			pass
		# 			# pix[x,y] = (0, 255,0)
		# 			#image.SetPixel(x, y, System.Drawing.Color.Green)
		# 		elif ( element == 2):
		# 			pass
		# 			# pix[x,y] = (0,0,255)
		# 			#image.SetPixel(x, y, System.Drawing.Color.Blue)
		# #image.Save(filename)
		# print "Done!"
	
	def searchAround(self,x,y,size):
		for j in range(y-size,y+size):
			for i in range(x-size,x+size):
				if (self.array[j * self.width_meters+i] != 0):
					return true
		return false
						
	def __arrayCoordToGps(self,x,y):
		return metersToGps(x)+self.left_bound, metersToGps(y)+self.bottom_bound
		
	def __connectLines(self,coords,value):
		array=self.array
		firstX = gpsToMeters(coords[0].lat-self.left_bound)
		firstY = gpsToMeters(coords[0].lon-self.bottom_bound)
		trailingPoint = Point((firstX,firstY))
		for i in range(1,len(coords)):
			leadingPoint = Point(self.__gpsToArrayCoord(coords[i].lat,coords[i].lon))
			self.__drawLine(trailingPoint,leadingPoint,value)
			trailingPoint = leadingPoint;
		self.__drawLine(leadingPoint,Point((firstX,firstY)),value)
	
	def markPolygon(self,gpsCoords):
		coords = [self.__gpsToArrayCoord(coord.lat,coord.lon) for coord in gpsCoords]
		self.__connectLines(gpsCoords,2)
		center = getCenterCoord(coords)
		self.__fill(center[0],center[1],2)
		
	def __drawLine(self,start,stop,value):
		array=self.array
		startX = int(start.x)
		stopX = int(stop.x)
		startY = int (start.y)
		stopY = int (stop.y)
		print "Drawing line from (",startX,",",startY,") to (",stopX,",",stopY,")"
		if ( stop.x != start.x):
			slope = (stop.y-start.y)/(stop.x-start.x);
		else:
			slope = 9999999999999999
		if ( abs(slope) < 1):
			step = 1
			if (startX > stopX):
				step = -1
			for x in range (startX, stopX+step, step):
				# print "Placing zero at %i,%i Slope: %f"%(x,int(step * slope*(x-startX))+startY,slope)
				self.array[x][int(slope * (x-startX)) + startY] = value
				
		else:
			step = 1
			if (startY > stopY):
				step = -1
			for y in range (startY, stopY+step, step):
				self.array[int((y-startY) / slope) + startX][y] = value
	
	def __fill(self,startX,startY,val):
		array = self.array
		pts = [(startX,startY)];
		while len(pts)>0:
			next = [];
			for (x,y) in pts:
				if not array[x][y] == val:
					#add four neighbors to newpts
					if x > 0:
						next.append((x-1,y))
					if y > 0:
						next.append((x,y-1))
					if x+1 < len(array):
						next.append((x+1,y))
					if y+1 < len(array[0]):
						next.append((x,y+1))
					array[x][y] = val
			pts = next

	# Function: getBounds()
	# Input: An array of GPS coordinates
	# Output: A tuple containing the bounds for the map
	# Side-Effects: None
	def __getBounds(self,coords):
		bottom_bound = coords[0].lon
		top_bound = coords[0].lon
		left_bound = coords[0].lat
		right_bound = coords[0].lat
		for coord in coords:
			if ( coord.lon < bottom_bound):
				bottom_bound = coord.lon
			if ( coord.lon > top_bound ):
				top_bound = coord.lon
			if ( coord.lat < left_bound ):
				left_bound = coord.lat
			if ( coord.lat > right_bound ):
				right_bound = coord.lat
		return bottom_bound - self.BORDER,top_bound + self.BORDER,left_bound - self.BORDER ,right_bound + self.BORDER;
		
	def testGetBounds():
		print("Testing getBounds()...")
		bottom_bound, top_bound, left_bound, right_bound = getBounds((GPSCoord(-5,-5,0), GPSCoord(5,-5,100), GPSCoord(5,-25,0), GPSCoord (0,0,0)));
		if ( bottom_bound != -25 - BORDER ):
			print "Bottom Bound Invalid: " , bottom_bound
		if ( top_bound != 0 + BORDER ):
			print "Top Bound Invalid: " , top_bound
		if ( left_bound != -5 - BORDER):
			print "Left Bound Invalid: " , left_bound
		if ( right_bound != 5 + BORDER):
			print "Right Bound Invalid: " , right_bound
		print "Testing Complete!"
		
	def testConnectLines():
		print "Testing connectLines()..."
		a = GPSCoord ( metersToGps(0), metersToGps(0) , 0 )
		b = GPSCoord ( metersToGps(10), metersToGps(10) , 0 )
		c = GPSCoord ( metersToGps(0), metersToGps(10) , 0 )
		d = GPSCoord ( metersToGps(10), metersToGps(0) , 0 )
		AutoSearch((a,b,c,d)).printSelf()

