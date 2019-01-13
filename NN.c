#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct NeuNet NeuNet;

/*
1:
Feedforward neural network, with the abiliy for back propagation 
Each index corresponds a node 

*/
struct NeuNet{
	
	//Each neural network has input and output links
	double* weight;
	NeuNet* next; //List of all the nodes in the next 	
	double a; //Either 1 or 0, depends on input csv values
};

//Global Varibales

//The single final node, if weight can be either 1 or 0, 1 when the vlock draws a seven and 0 otherwise
NeuNet FINALNODE = {.weight = NULL, .next = NULL};
//const double WEIGHT = 100; //Initial weight we set the node values to have
const int NUMINPUT = 9; //Number of input nodes
double ALPHA; //Variable that changes speed of learning though back propagation.


/*
Setting up the input nodes, the values are determined by the csv 
*/
void createInputNodes(NeuNet* inputNodes, NeuNet* middleNodes, double* inputAValues, int numNodes){
	 
	//Iterate through to initializa values
	for(int i=0;i<NUMINPUT;i++){
		double* initialWeight = malloc(numNodes*sizeof(double));
	//		int random = rand()%20;
		//Initialize all the weight values
		for(int j=0;j<numNodes;j++){
			
			initialWeight[j] = 1.0/(double)((rand()%50)+0.0001);//(double)(1.0/(rand()%20)); //Number between 1 and 1/20
		}	
		NeuNet node = {.weight = initialWeight,.next = middleNodes,.a=inputAValues[i]};
		inputNodes[i] = node;		
	}
}

/*
Update the input and output values to train the next set
*/
void updateValues(NeuNet* inputNodes, double* inputAValues, double isSeven){

	//Iterate through the nodes to update the values
	for(int i=0; i<NUMINPUT;i++){
		inputNodes[i].a = inputAValues[i];
	}

}

/*
Middle Nodes only link to the one finalNode, with their weights initialized to 1 and the bias to 0 (subject to change)
The numNodes is gotten from the command line args and determines how many middle nodes there should be.
*/
void createMiddleNodes(int numNodes, NeuNet* middleNodes){

//	int random = rand()%20;
	//Iterate through to intialize every middle node
	for(int i=0;i<numNodes;i++){
		//Setup the intial values of each node
		double* initialWeight = malloc(sizeof(double));
		initialWeight[0] = (1.0/(double)((rand()%50)+0.001)); // Num between 0 and 1
		NeuNet node = {.weight = initialWeight, .next = &FINALNODE};
		
		//Add the node to the array of middle nodes
		middleNodes[i] = node;		
	}

}

/*
Free the weight and next values of the nodes that have been allocated during the program
*/
void freeWeights(NeuNet* inputNodes, NeuNet* middleNodes, int numNodes){
	
	
	free(inputNodes[0].next);
	for(int i=0;i<NUMINPUT;i++){
		free(inputNodes[i].weight);
	}

	for(int i=0;i<numNodes;i++){
		free(middleNodes[i].weight);
	}


}
/*
Computes the sigmoid activation function 
*/
double sigmoid(double in){

	double denom = 1 + exp((-1*in));
	return (double)(1.0/denom); 
}

/*
Computes the derivative of the sigmoid activation function
*/
double sigmoidDiv(double in){

	double sigmoidVal = sigmoid(in); //reduce the number of calls to sigmoid

	return sigmoidVal*(1-sigmoidVal);
}

/*
Propagate though the layers and set up the activation function and change the a values in the node. 
NumNodes is the number of nodes in the middle layers
*/
void nodeActivation(NeuNet* inputNodes, NeuNet* middleNodes, int numNodes){
	
	
	double in = 1.0; //Value that goes in the activation function, sum of all inputs * weights
	//Iterate through each of the middle nodes to change the a values
	for(int i=0;i<numNodes;i++){
		
		in = 0.0; //Reset the value for the next middle node
		//Go through all in input nodes to get the sum
		for(int j=0;j<NUMINPUT;j++){
			NeuNet curNode = inputNodes[j]; // Clean-up the code
			in += curNode.weight[i]*curNode.a;
			
		}
		//Update the a values for all nomes
		middleNodes[i].a = sigmoid(in); 		
	}

	in = 0.0;
	//End node activation
	for(int i=0;i<numNodes;i++){
		in += middleNodes[i].weight[0]*middleNodes[i].a;
	
	}
	//Update the final node a value
	FINALNODE.a = sigmoid(in);	
}


/*
Go backwards from the final node to back propagate and change the
values of the weights. Uses global variable ALPHA for speed of change
*/
void backProp(double isSeven, int numNodes, NeuNet* middleNodes, NeuNet* inputNodes){

	//Start with the output layer
	//End node in value (sum of all weights times the a values)
	double in = 0.0;
	for(int i=0;i<numNodes;i++){
		in += middleNodes[i].weight[0]*middleNodes[i].a;
	
	}
	//Update the final node a value
	double deltaJ;
	deltaJ = sigmoidDiv(in)*((double)isSeven-FINALNODE.a);
//	printf("DeltaJ is %f\n", deltaJ);	
	//Go through the middle nodes
	double* deltaI = malloc((numNodes+1)*(sizeof(double)));
	double sum = 0.0; //SUm of the weight times the deltaJ for all nodes
	//Iterate through each of the middle nodes to change the a values
	for(int i=0;i<numNodes;i++){
		
		in = 0.0; //Reset the value for the next middle node
		sum = 0.0;
		//Go through all in input nodes to get the sum
		for(int j=0;j<NUMINPUT;j++){
			NeuNet curNode = inputNodes[j]; // Clean-up the code
			in = in + curNode.weight[i]*curNode.a;
			sum = sum + curNode.weight[i]*deltaJ;
		}
		//Update the a values for all nomes
		deltaI[i] = sigmoidDiv(in)*sum; 		
	//	printf("DeltaI for middle node %d is %f, sum is \t%f\n", i, deltaI[i], sum);
	}

	//Update the weights from input to middle
	for(int i=0;i<NUMINPUT;i++){
		for(int j=0;j<numNodes;j++){
			NeuNet curNode = inputNodes[i]; //CLean up code
			curNode.weight[j] = curNode.weight[j] + ALPHA*curNode.a*deltaI[j];	
	//		printf("Change in weight from input node %d to middle node %d is: %f\n", i, j, ALPHA*curNode.a*deltaI[j]);	
		}
	}
	
	//Update the weights from middle to output nodes
	for(int i=0;i<numNodes;i++){
		//NeuNet curNode = middleNodes[i];
		//curNode.weight[0] = curNode.weight[0] + ALPHA*curNode.a*deltaJ;
		middleNodes[i].weight[0] += ALPHA*middleNodes[i].a*deltaJ;
//		printf("For middle node %d, a: %f, ALPHA: %f, deltaJ: %f, currentWeight: %f\n",i,curNode.a,ALPHA,deltaJ,curNode.weight[0]);
	}
	free(deltaI);
} 
/*
Parse the csv file to remove the commas, returns a dynamic string that contains ints
Returns the array of ints corresponding to the input values. Indices 0-8 are the block values and index 9 is where the entire block is a 7 or not.
*/
void parseLine(char* line, double* inputValues){

	//Final array of input values that we are returning
	char* values = malloc((NUMINPUT+1)*sizeof(char));
	//delimeter pointer
	const char s[2] = ",";

	values = strtok(line, s);

	int i = 0;
	while(values != NULL){
		
		//printf("%s\n", inputValues);
	
		inputValues[i] = (double)atoi(values); 	
		values = strtok(NULL,",");
		i = i + 1;	
	}
	
	free(values);
}

/*
Print out the entire Node architecture with all the internal values, goes from input to output
*/

void printNeuNet(NeuNet* inputNodes,int numNodes){

	printf("INPUT NODES:\n");
	//Print the inputNodes
	for(int i=0;i<NUMINPUT;i++){
		NeuNet curNode = inputNodes[i]; //Clean up code
		printf("Input node %d has an 'a' value of: %f\n", i, curNode.a);
		//Print the weights
		for(int j=0;j<numNodes;j++){	
			printf("Input node %d to middle node %d has weight of %f\n", i, j, curNode.weight[j]); 
		}
		printf("\n"); 
	}
	
	//Print the middle nodes
	printf("\nMIDDLE NODES:\n");

	NeuNet* middleNodes = inputNodes[0].next;
	for(int i=0;i<numNodes;i++){
		NeuNet curNode = middleNodes[i]; //Clean up code
		printf("Middle node %d has an 'a' value of: %f\n \tit also has a weight to final node of %f\n",i, curNode.a,curNode.weight[0]);
		
	}

	//Print output node
	printf("\nOUTPUT NODES\nOutput node final value is: %f\n\n", FINALNODE.a);	
}

/*
Activate the next step in the Neunetwork, change input values and change the weights
*/
void updateNetwork(NeuNet* inputNodes, NeuNet* middleNodes, int numNodes, double* inputAValues){
	double isSeven = inputAValues[NUMINPUT];
	updateValues(inputNodes, inputAValues, isSeven);
	nodeActivation(inputNodes, middleNodes, numNodes);
	backProp(isSeven,numNodes,middleNodes,inputNodes);
	nodeActivation(inputNodes, middleNodes, numNodes);	
//	printNeuNet(inputNodes,numNodes);	
} 

 
/**

Neural network is feedfoward with 3 layers, 1st layer is 9 values 1 to 0 second layer size is gotten from the csv file. The input layer corresponds to the values of the blocks and the output node should be a 1 if the blocks draw a 7 and 0 otherwise. 

**/

int main(int argc, const char** argv){

	//Check arg values
	if((argc != 4)|| (atoi(argv[1]) <= 0)){
		printf("Arguments are not valid\n");
		return 0;
	}	

	//How many nodes for the middle layer
	int numNodes = atoi(argv[1]);
	//How many lines of there training set there is
	int trainSize = atoi(argv[2]);	

	ALPHA = atof(argv[3]);
	//Gather the input values from the csv file
	FILE *file;
	
	char* filename = "TrainingSet.csv";
	//*filename = "NeuNetworkTest.csv";
	file = fopen(filename, "r");
	
	//The first line on the csv file
	char* line = malloc(4098*sizeof(char));
	fgets(line,4098,file);
	char* tmp = strdup(line);
	//printf("The thing: %s\n", tmp);

	//Gather the input files from the csv
	double* inputAValues = malloc((NUMINPUT+1)*sizeof(double));
	parseLine(tmp, inputAValues);		

	
	//Last value in the line is a boolean int if it is a seven
	//double isSeven = (double)inputAValues[NUMINPUT];
	//Create the middle nodes, there is only one output node (finalNode)
	
	NeuNet* middleNodes = malloc(numNodes*sizeof(NeuNet));
	createMiddleNodes(numNodes,middleNodes);

	//printf("last node weight value: %f\n", middleNodes[numNodes-1].weight[0]);
	//Creating the input nodes
	NeuNet* inputNodes = malloc(NUMINPUT*sizeof(NeuNet));
	createInputNodes(inputNodes,middleNodes,inputAValues,numNodes);
	
	//	printf("last node weight value: %f\n", middleNodes[numNodes-1].weight[0]);
	//Update the neunet line by line
	 
	for(int i=0; i<trainSize;i++){	
		updateNetwork(inputNodes,middleNodes,numNodes, inputAValues);
		
//		printNeuNet(inputNodes,numNodes);
		fgets(line,4098,file);
		tmp = strdup(line);
		parseLine(tmp, inputAValues);
		
	}


	//Test the results
/*	char* test = "1,1,1,0,0,1,0,0,1,1";
	inputAValues = parseLine(test);
	updateValues(inputNodes, inputAValues, isSeven);
	nodeActivation(inputNodes, middleNodes, numNodes);
	printNeuNet(inputNodes,numNodes);	
*/
	
	fclose(file);
	
	filename = "TestSet.csv";
	//Print the expected and actual values to chec the network
	FILE* check = fopen(filename, "r");
	char* line2 = malloc(4098*sizeof(char));
	if(trainSize>30){
		trainSize = 30;
	}
	for(int i=0;i<trainSize;i++){
		fgets(line2,4098,check);
		tmp = strdup(line2);
		parseLine(tmp, inputAValues);
		updateValues(inputNodes, inputAValues, inputAValues[NUMINPUT]);
		nodeActivation(inputNodes, middleNodes, numNodes);
		
		printf("For line %d the value should be  \t%f   \t The real value: \t%f\n\n", i+1, inputAValues[NUMINPUT], FINALNODE.a);		
	//	printf("--------------------------------------------------------------------------------------------------------\n");
	}

//	fclose(check);
	//Realeasing the pointers 
//	freeWeights(inputNodes,middleNodes,numNodes);
	free(file);
	free(tmp);
	free(line);
	free(inputAValues);
	free(middleNodes);
	free(inputNodes);
	
	return 0;

}

