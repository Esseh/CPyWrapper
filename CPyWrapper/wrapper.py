resultMap = {}
argMap = {}

class resultObject:
	def __init__(self,error,result):
		self.error = error
		self.result = result

# Wraps the exceution of a python function automatically catching the error and passing it up in a C++ readable format.
def executeFunction(function,args):
	global resultMap
	global argMap
	argMap[function.__name__] = args
	try:
		# If nothing goes wrong then return no error along with the result of the function
		result = resultObject("",function(*args))
		resultMap[function.__name__] = result
		return result
	except Exception as e:
		# Otherwise pass the error up in a format usable by C++.
		errorType = type(e)
		truncatedErrorType = str(errorType)[7:-2]
		result = resultObject(truncatedErrorType, None)
		resultMap[function.__name__] = result
		return result
		
def tostring(a,b):
	return str(a) + str(b)