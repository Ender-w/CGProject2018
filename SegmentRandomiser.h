
#ifndef _SEGMENTRANDOMISER_H_
#define _SEGMENTRANDOMISER_H_

/**
 * Function to create a random map segment
 * In a seperate file due to its length
 * Author Daniel - a1688639
 */
vector< vector<int> > chooseRandomSegment(){

	int numberOfSegments = 22;
	int segmentID = rand() % numberOfSegments + 1;
	
	if(segmentID == 1){//I wanted to do a switch but i couldnt with the uniform initialisation :(
		vector< vector<int> > segment {	{1,1,1},
										{1,0,0},
										{1,0,0}};
		return segment;
	}else if(segmentID == 2){
		vector< vector<int> > segment {	{1,1,1},
										{0,0,1},
										{0,0,1}};
		return segment;
	}else if(segmentID == 3){
		vector< vector<int> > segment {	{0,1,0},
										{0,1,0},
										{0,1,0}};
		return segment;
	}else if(segmentID == 4){
		vector< vector<int> > segment {	{1,1,1},
										{1,1,0},
										{1,1,0}};
		return segment;
	}else if(segmentID == 5){
		vector< vector<int> > segment {	{1,1,1},
										{0,1,1},
										{0,1,1}};
		return segment;
	}else if(segmentID == 6){
		vector< vector<int> > segment {	{1,1,1},
										{1,0,1},
										{1,0,1}};
		return segment;
	}else if(segmentID == 7){
		vector< vector<int> > segment {	{1,0,1},
										{1,0,1},
										{1,0,1}};
		return segment;
	}else if(segmentID == 8){
		vector< vector<int> > segment {	{1,1,1},
										{0,1,0},
										{0,0,0}};
		return segment;
	}else if(segmentID == 9){
		vector< vector<int> > segment {	{1,1,1},
										{1,1,1},
										{0,0,0}};
		return segment;
	}else if(segmentID == 10){
		vector< vector<int> > segment {	{1,1,1},
										{1,1,1},
										{0,0,1}};
		return segment;
	}else if(segmentID == 11){
		vector< vector<int> > segment {	{1,1,1},
										{1,1,1},
										{0,1,0}};
		return segment;
	}else if(segmentID == 12){
		vector< vector<int> > segment {	{1,1,1},
										{1,1,1},
										{1,0,0}};
		return segment;
	}else if(segmentID == 13){
		vector< vector<int> > segment {	{0,0,0},
										{0,0,0},
										{1,1,1}};
		return segment;
	}else if(segmentID == 14){
		vector< vector<int> > segment {	{1,0,0},
										{1,0,0},
										{1,1,1}};
		return segment;
	}else if(segmentID == 15){
		vector< vector<int> > segment {	{0,1,0},
										{0,1,0},
										{1,1,1}};
		return segment;
	}else if(segmentID == 16){
		vector< vector<int> > segment {	{0,0,1},
										{0,0,1},
										{1,1,1}};
		return segment;
	}else if(segmentID == 17){
		vector< vector<int> > segment {	{1,0,1},
										{1,0,1},
										{1,1,1}};
		return segment;
	}else if(segmentID == 18){
		vector< vector<int> > segment {	{0,1,1},
										{0,1,1},
										{1,1,1}};
		return segment;
	}else if(segmentID == 19){
		vector< vector<int> > segment {	{1,1,0},
										{1,1,0},
										{1,1,1}};
		return segment;
	}else if(segmentID == 20){
		vector< vector<int> > segment {	{1,1,0},
										{1,1,0},
										{0,0,1}};
		return segment;
	}else if(segmentID == 21){
		vector< vector<int> > segment {	{0,1,1},
										{0,1,1},
										{1,0,0}};
		return segment;
	}else if(segmentID == 22){
		vector< vector<int> > segment {	{0,1,0},
										{0,1,0},
										{1,0,1}};
		return segment;
	}else{
		cerr << "Invalid segmentID " << segmentID << endl;
	}
	
}

#endif // _SEGMENTRANDOMISER_H_
