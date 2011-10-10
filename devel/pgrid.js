// ##############################
// #
// # Let's pseudo-code a grid-search pattern
// # Basically, move in a straight vecotr (+-1,0 or 0,+-1) until encounter [something]
// # If we still are executing, means that [something] wasn't our goal, so decide
// # on new vector.
// # If vector was (1,0),  change vector to (0,1) for one step, then (-1,0)
// # If vector was (0,1),  change vector to (1,0) for one step, then (0,-1)
// # If vector was (-1,0), change vector to (...) ...
// # ...
// #
// # For this proc, we need:
// # - Last known vector 
// # - Sensor Input (e.g. circle of objects around, ANSITextSensor)
// # 	- for use with "can we move in dir of vector?"
// #
// # Output: SimpleXYMovementVector Node
// #
// # Store in context: Last Known Vector
// #
// ##############################


// proc	=> $proc,
// context	=> $context,
// agent	=> $agent,
// input_nodes	=> $input_nodes,

var ctxNode = context.node();

// SimpleXYMovementVector stores (x,y) as attributes of the data() function:
// var x = lastKnownVector.data().get("x");
var lastKnownVector = ctxNode.linked_node(Types.SimpleXYMovementVector);

var vectorData = lastKnownVector.data();

// SimpleTextVisualContext store visual context as data hanging off it in eight different
// attributes named, appropriatly: 
// N,S,E,W,NE,SE,NW,SW
// or
// 0-1 01 01 0-1 1-1 11 -1-1 -11
// Both keys are valid.
// The data stored in those atributes is a hash of {char: "character", attr:[,,,,]} where attr is a list of flags/values for ansi attributes
// visualContext should be current as of the LAST move
var visualContext = ctxNode.linked_node(Types.SimpleTextVisualContext);

var vectorStr = parseInt(vectorData.get('x'))+","+parseInt(vectorData.get('y'));

//print("Debug: vectorStr: ", vectorStr);
//print("Visual Context: ",visualContext);
print("Old Vector: ",vectorData.get('x'),",",vectorData.get('y'));

var visualInfo = visualContext.data().get(vectorStr);
if(visualInfo)
{
	//print("Saw something at "+vectorStr+", Character '", visualInfo.char, "', attr: ", visualInfo.attr);
	print("Saw something at "+vectorStr+", Character '", visualInfo, "'");
// 	if(vectorData.get("inTurn"))
// 	{
// 	
// 	}
// 	
	if(vectorStr == "1,0")
	{
		vectorData.set({x:0,y:1, turn:"-1,0"});	
	}
	else
	if(vectorStr == "0,1")
	{
		vectorData.set({x:-1,y:0, turn:"0,-1"});
	}
	else
	if(vectorStr == "-1,0")
	{
		vectorData.set({x:0,y:-1, turn:"1,0"});
	}
	else
	if(vectorStr == "0,-1")
	{
		vectorData.set({x:1,y:0, turn:"0,1"});
	}
	
	print("New Vector: ",vectorData.get('x'),",",vectorData.get('y'));
}
else
{
	var alt = Math.random() > 0.5 ? "1,0" : "";
	var inTurn = vectorData.get('turn');
	if(inTurn == "-1,0")
		vectorData.set({x:-1,y:0,turn:alt});
	else
	if(inTurn == "0,-1")
		vectorData.set({x:0,y:-1,turn:alt});
	else
	if(inTurn == "1,0")
		vectorData.set({x:1,y:0,turn:alt});
	else
	if(inTurn == "0,1")
		vectorData.set({x:0,y:1,turn:alt});
	
	print("No visual info at "+vectorStr);
}

// if(vectorData.is_changed())
// 	vectorData.update();
	
// print("Changing last known vector to 01");
// vectorData.update({x:0,y:1});

return lastKnownVector;
