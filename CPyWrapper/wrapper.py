class resultObject:
	def __init__(self,error,result):
		self.error = error
		self.result = result

# Wraps the exceution of a python function automatically catching the error and passing it up in a C++ readable format.
def executeFunction(function,args):
	try:
		# If nothing goes wrong then return no error along with the result of the function
		return resultObject("",function(*args))
	except Exception as e:
		# Otherwise pass the error up in a format usable by C++.
		errorType = type(e)
		truncatedErrorType = str(errorType)[7:-2]
		return resultObject(truncatedErrorType, None)