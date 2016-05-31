// AIp4final.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <time.h>
using namespace std;

//prototypes
int aiMove(int[], int[], int, int, int, time_t, time_t, int);
int cWin(int[], int);
int wIt(int[], int, int, int, int, int, int);
int detMove(int[], int[], int);
int range(int[], int, int, int, int);

int main(int argc, char* argv[])
{
	time_t start = 0; //start time
	time_t end = 0; //end time
	int tt = atoi(argv[2]); //time to execute search
	int s = 56; //board size
	int board[56]; //board
	int pm[8] = {48, 49, 50, 51, 52, 53, 54, 55}; //open moves
	int p = atoi(argv[1]); //player to decide for
	int m = 0; //move
	for(int i=0; i<s; i++){ //initialize board
		board[i] = atoi(argv[i+3]);
		if(board[i]!=0) pm[i%8]-=8;
	}
	m = aiMove(board, pm, p, 1, 3, start, end, tt); //call aimove
	cout << m << endl;
	return m;
}

//function to perform iterative deepening search and return best move
int aiMove(int b[], int pm[], int p, int d, int md, time_t s, time_t e, int tt){
	if(d==1) time(&s); //initialize start time
	bool clear = true; //determine if possible threats have been seen
	int w = cWin(b, 55); //check for winner
	if(w!=0 || d==md) return w; //return win value if not 0 or max depth
	int pv, t, bvi, prev, previ = 0; //player, return value of search, column of best value, previous search best value, previous search best value column
	if(p==1) pv=2; //set player oppenent
	else pv = 1;
	int bv = pv; //best value initially loss
	//if no moves return win status
	int c=-1; 
	for(int k=0; k<8; k++) if(pm[k]>c) c=pm[k];
	if(c==-1) return w;
	for(int i=0; i<8; i++){ //main iteration for search
		if(pm[i]>=0){ //search only open columns
			b[pm[i]]=p; //set move
			pm[i]-=8; //set open slot
			t = aiMove(b, pm, pv, d+1, md, s, e, tt); //search
			pm[i]+=8; //unset slot
			b[pm[i]]=0; //unset move
			if(d==1 && t!=0) clear = false; //check threat depth 1
			if(d==1 && t==p) return i; //return if win depth 1
			if(t==p) return p; //return winning player if win
			//update values
			else if(t==0){ bv=t;
				bvi=i;}
			else if(bv==pv){bv=t;
				bvi=i;}
			//increase depth if time permits depth 1
			if(d==1 && i==7){
				time(&e);
				if(difftime(e, s)<tt-1){ 
					md++;
					i=-1;
					prev = bv;
					previ = bvi;
				}
			}
			//terminate and take previous searches values if time reached
			time(&e);
			if(d==1 && difftime(e, s)>=tt-1){
				bv = prev;
				bvi = previ;
				i=8;
			}
			//return if time up and not depth 1
			if(d!=1 && difftime(e, s)>=tt-1) return 0;
		}
	}
	if(d==1 && !clear) return bvi; //return value if threat seen depth 1
	if(d==1 && clear) return detMove(b, pm, p); //return move with highest heuristic value if no threat depth 1
	return bv; //return best value
}

//function to check for winner
int cWin(int b[], int s){
	int basev = 0; //iteration base value
	int wv = 0; //iterate vertical win check
	int wl = 0; //iterate lateral win check
	int d = 1; //distance from base on given line
	//horizontal check vars
	int c = 0;
	int t = 0;
	//check vertical and lateral wins
	for(int i=24; i<32; i++){
		if(b[i]!=0){
			wv = wl = i;
			basev = b[i];
			d = wIt(b, s, i, basev, wv, d, 8);
			if(d>1) return basev;
			d = wIt(b, s, i, basev, wv, d, -7);
			if(d>1) return basev;
			d = wIt(b, s, i, basev, wv, d, 9);
			if(d>1) return basev;
		}
	}
	//check horizontal wins
	for(int i=0; i<49; i+=8){
		t=0;
		c=1;
		for(int k=i; k<i+8; k++){
			if(b[k]==t)c++;
			else{
				t=b[k];
				c=1;
			}
			if(c>3 && t!=0) return t;
		}
	}
	return 0;
}

//function for checking vertical and lateral wins
int wIt(int b[], int s, int base, int basev, int w, int d, int it){
	int l = 4;
	int r = 4;
	if(it%8!=0){
		l = 0;
		int t = base;
		while((base-l)%8!=0) l++;
		r = 7-l;
		if(l>3) l = 3;
		if(r>3) r = 3;
	}

	for(int i=0; i<l; i++){
		w-=it;
		if(b[w]==basev) d++;
		else break;
	}
	w = base;
	for(int i=0; i<r; i++){
		w+=it;
		if(b[w]==basev) d++;
		else break;
	}
	if(d>=4) return d;
	else return 1;
}

//heuristic function
int detMove(int b[], int pm[], int p){
	int pv;
	if(p=1) pv=2;
	else pv=1;
	int base = 48;
	int t=base;
	int d = 0;
	int ru = 1; //range up
	int rd = 1; //range down
	int vv = 0; //vertical value
	int hv = 0; //horizontal value
	int duv = 0; //diagonal up value
	int ddv = 0; //diagonal down value
	int tv[8]; //total values
	int h = 1;

	//determine move values
	for(int i=0; i<8; i++)
	{
		if(pm[i]>-1){
		t = pm[i];
		//horizontal
		ru = range(b, t, 1, 0, p);
		rd = range(b, t, -1, 7, p);
		for(int k=t-rd; k<=t+ru; k++){
			if(k==pm[i]) hv+=h;
			else if(b[k]==p) hv+=h;
			else hv+=1;
		}
		
		//vertical
		ru = range(b, t, -8, 9, p);
		rd = range(b, t, 8, 9, p);
		for(int k=t+(8*rd); k>=t-(8*ru); k-=8){
			if(k==pm[i]) vv+=h;
			else if(b[k]==2) vv+=h;
			else vv+=1;
		}

		//d7
		ru = range(b, t, -7, 0, p);
		rd = range(b, t, 7, 7, p);
		for(int k=t+(7*rd); k>=t-(7*ru); k-=7){
			if(k==pm[i]) duv+=h;
			else if(b[k]==2) duv+=h;
			else duv+=1;
		}

		//d9
		ru = range(b, t, 9, 0, p);
		rd = range(b, t, -9, 7, p);
		for(int k=t+(9*ru); k>=t-(9*ru); k-=9){
			if(k==pm[i]) ddv+=h;
			else if(b[k]==2) ddv+=h;
			else ddv+=1;
		}
		hv-=3;
		vv-=3;
		duv-=3;
		ddv-=3;
		tv[i] = hv+vv+duv+ddv;
		hv=vv=duv=ddv=0;
		}
	}
	int best = 0;
	for(int i=0; i<8; i++){
		if(tv[best]<tv[i]) best = i;
	}
	return best;
}

//function to get a given positions open range, used by heuristic function
int range(int b[], int t, int it, int stop, int p){
	int pv;
	if(p=1) pv=2;
	else pv=1;
	int d=0;
	while(t%8!=stop){
		if(b[t]!=pv)d++;
		else{if(d==0) return 0;
			else if(d<4) return d-1;
			else return 3;} 
		t+=it;
		if(t>55 || t<0) break;
	}
	if(d==0) return 0;
	else if(d<4) return d-1;
	else return 3;
}