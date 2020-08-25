#include "abc.h"
#include <iostream>
#include <fstream>
#include <algorithm> 
#include <sstream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <stdlib.h>
#include <map>
#include <cassert>
#include <time.h>
#include <vector>
#include <random>
#include <omp.h>
#include <numeric>
#define USE_OPENMP TRUE

#ifdef USE_OPENMP
#endif

#define DETAIL_VERBOSE TRUE
#define SHORT_TERM_RATE 0.1
#define BIASED_MEAN_FACTOR 0.1
#define BIASED_MEAN TRUE
using namespace std;

// create default opts need to be tuned
// todo: create a global for testing 
//      && a new function that parses an input file (containing the opts)
vector<string> default_options()
{
    vector<string> default_opts;
    default_opts.push_back("rewrite");
    default_opts.push_back("rewrite -z");
    //default_opts.push_back("balance");
    default_opts.push_back("refactor");
    default_opts.push_back("refactor -z");
    default_opts.push_back("resub -K 8");
    //default_opts.push_back("resub -K 8 -z");
    default_opts.push_back("dc2");
    return default_opts;
}

//cnf tuning
vector<string> default_options3()
{
    vector<string> default_opts;
    default_opts.push_back("rewrite -l");
    default_opts.push_back("rewrite -z -l");
    default_opts.push_back("balance -l");
    default_opts.push_back("refactor -l");
    default_opts.push_back("refactor -z -l");
    default_opts.push_back("resub -K 8 -l");
    default_opts.push_back("resub -K 8 -l -z");
    default_opts.push_back("dc2");
    return default_opts;
}

vector<string> default_options2()
{
    vector<string> default_opts;
    default_opts.push_back("rewrite");
    default_opts.push_back("rewrite -z");
    default_opts.push_back("balance");
    default_opts.push_back("refactor");
    default_opts.push_back("refactor -z");
    default_opts.push_back("resub -K 6");
    return default_opts;
}


vector<string> split(string str, string token){
    vector<string>result;
    while(str.size()){
        int index = str.find(token);
        if(index!=string::npos){
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.size()==0)result.push_back(str);
        }else{
            result.push_back(str);
            str = "";
        }
    }
    return result;
}
// softmax function
static void softmax(vector<float> &input, int input_len)
{
    assert (input_len != 0);
    int i;
    float m;
    /* Find maximum value from input array */
    m = input[0];
    for (i = 1; i < input_len; i++) {
        if (input[i] > m) {
            m = input[i];
        }
    }
    float sum = 0;
    for (i = 0; i < input_len; i++) {
        sum += expf(input[i]-m);
    }

    for (i = 0; i < input_len; i++) {
        input[i] = expf(input[i] - m - log(sum));
    }    
}

// logsoftmax function
static void logsoftmax(vector<float> &input, int input_len)
{
    assert (input_len != 0);
    int i;
    float m;
    /* Find maximum value from input array */
    m = input[0];
    for (i = 1; i < input_len; i++) {
        if (input[i] > m) {
            m = input[i];
        }
    }
    float sum = 0;
    for (i = 0; i < input_len; i++) {
        sum += expf(input[i]-m);
    }

    for (i = 0; i < input_len; i++) {
        input[i] = (expf(input[i] - m - log(sum)));
    }    
}

// random generator function:
int myrandom (int i) { return std::rand()%i;}

double norm_dist_num(){
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(5.0,2.0);
    return distribution(generator); 
}

// clean the string of recommended flow
string clean_grep(string recmd)
{
    int find_grep = 0;
    string output;
    find_grep = recmd.find("|");
    output.assign(recmd, 0, find_grep);
    return output;
}

// clean the string of recommended flow
string clean_grep_2(string recmd)
{
    int find_grep = 0;
    string output,temp;
    temp = clean_grep(recmd);
    output.assign(temp, 8, temp.length()-2-8);
    return output;
}


// clean the string of recommended flow
string clean_flow_only_cmd(string recmd)
{
    string output;
    output = clean_grep(recmd);
    int find_start = 0;
    find_start = output.find("\"");
    string cmd;
    int find_end = 0;
    find_end = output.find("\"", find_start+1);
    cmd.assign(output, find_start+1, find_end-find_start-1);
    return cmd;
}

// clean the string of recommended flow
string clean_flow_only_cmd_yosys(string recmd)
{
    string output;
    output = clean_grep(recmd);
    int i0 = 0;
    i0 = output.find(";",0);
    int i1 = 0;
    i1 = output.find(";", i0);
    string cmd;
    int find_end = 0;
    find_end = output.find("\"", i1+1);
    cmd.assign(output, i1+1, find_end-i1-1);
    return cmd;
}

// clean the string of recommended flow
string clean_flow_no_mapping(string recmd)
{
    string output;
    output = clean_grep(recmd);
    int i0 = 0;
    i0 = output.find(";",0);
    int i1 = 0;
    i1 = output.find(";", i0);
    string cmd;
    int find_end = 0;
    find_end = output.find("\"", i1+1);
    cmd.assign(output, i1+1, find_end-i1-1);
    i0 = cmd.find("ifraig",0);
    string cmd_no_map;
    cmd_no_map.assign(cmd, 0, i0);
    return cmd_no_map;
}


// generate random command
string random_opts(vector<string> default_opts, int repeats){
    vector<int> vecIndex;
    for(int r=0;r<repeats;r++)
        for(int i=0;i<default_opts.size();i++)
            vecIndex.push_back(i);
    random_shuffle ( vecIndex.begin(), vecIndex.end());
    //random_shuffle ( vecIndex.begin(), vecIndex.end(), myrandom);
    string command;
    for(int i=0;i<repeats*default_opts.size();i++)
    {
        command += default_opts[vecIndex[i]];
        if(i!=repeats*default_opts.size()-1)
            command+=";";
    }
    return command;
}

int factorial(int n)
{
      return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}
int choose_n_k(int n,int k)
{
      int r=1;
      for(int i=n;i>n-k;i--){
        r=r*i;
      }
      return r;
}

/* Generate random optization sequence (commands) 
    vector<int> v                  :   encode in a integer vector; each element indicates the position of an opt
    vector<string> default_opts    :   default opts vector */
string vec2command(vector<int> v, vector<string> default_opts)
{
    string command;
    for(int i=0;i<v.size();i++)
    {
        command += default_opts[v[i]];
        if(i!=v.size()-1)
            command+=";";
    }
    return command;
}

/* generate constrained random command  
        vector<string> default_opts         :
        int repeats                         : number of appearances
        int pos                             : prefix positions (will be used for exhaustive cases splitting */
string constrain_random_opts(vector<string> default_opts, int repeats, int pos){
    vector<int> vecIndex;
    // generated ordered sequnce vector (index vector)
    // e.g., 3 opts + 2 repeats => 0 1 2 0 1 2
    for(int r=0;r<repeats;r++)
        for(int i=0;i<default_opts.size();i++)
            vecIndex.push_back(i);
    // erase the prefix positions
    // e.g., if first two are fixed; 0 1 are erased; 2 0 1 2 will then be used to generated random sequences
    vecIndex.erase(vecIndex.begin(), vecIndex.begin()+pos);
    random_shuffle ( vecIndex.begin(), vecIndex.end());
    string command;
    command = vec2command(vecIndex, default_opts);
    return command;
}


// generate random commands
vector<string> random_commands(vector<string> default_opts, int repeats, int nCommands){
    vector<string> random_c;
//#pragma omp parallel for  // depended 
    for (int i=0; i<nCommands; i++){
        string command = random_opts(default_opts,repeats);
        if(find(random_c.begin(), random_c.end(), command) == random_c.end() )
            random_c.push_back(command);
    }
    return random_c;
}

/* generate prefix initial commands; used for generating constrained commands
    e.g., 0 1 2 0 1 2 ; pos = 2;
        then 2 0 1 2 will be used for random sampling
             0 1 will be used for all permutation 
             1 0 + randoms or 0 1 + randoms */
vector<string> create_headers1(int pos, vector<string> default_opts){
    vector<int> vecIndex;
    for(int i=0;i<pos;i++)
        vecIndex.push_back(i);

    vector<string> head; 
    do {
        string command = vec2command(vecIndex,default_opts);
        head.push_back(command);
    } while ( next_permutation( vecIndex.begin(), vecIndex.end() ) );
    
    return head;
}

/* generate prefix initial commands; used for generating constrained commands
    e.g., 0 1 2 0 1 2 ; pos = 2;
        then 2 0 1 2 will be used for random sampling
             0 1 will be used for all permutation 
             1 0 + randoms or 0 1 + randoms */
vector<string> create_headers2(int pos, vector<string> default_opts){
    vector<int> vecIndex;
    for(int i=0;i<default_opts.size();i++)
        vecIndex.push_back(i);

    vector<string> head; 
    do {
        vector<int> first_pos; 
        for(int i=0;i<pos;i++)
            first_pos.push_back(vecIndex[i]);
        string command = vec2command(first_pos,default_opts);
        head.push_back(command);
    } while ( next_permutation( vecIndex.begin(), vecIndex.end() ) );
    auto i = std::unique (head.begin(), head.end());
    head.resize( std::distance(head.begin(),i) );
    //for(auto& i : head)
    //    cout<<i<<endl;
    return head;
}

/* generate prefix initial commands; used for generating constrained commands */
/* exhaustive perm with headers2 */
vector<string> create_headers(int pos, vector<string> default_opts){
    vector<string> v;
    v = create_headers2(pos, default_opts); 
    //v = create_headers2(pos, default_opts); 
    return v;
}


// generate Quality-of-Result commands
// -- which_opt=0/1 => AIG minimization;  so it will append "strash;print_stats"
// -- which_opt=2/3 => STA tech-map tuning;  so it will append "upsize;dnsize;stime" to perform Gate-sizing and STA evaluation
// -- which_opt=4/5 => FPGA tech-map tuning;  so it will append "if -K 6;print_stats" to get levels and number of LUTs 
string abc_stats_commmands(int h, int i, int which_opt)
{
    if (which_opt == 0 || which_opt == 1) //AIG minimization
        return ";strash;ifraig;dch -f;strash;print_stats;\" | grep \"and =\" > " + to_string(h)+"_"+to_string(i)+".result\n";
    else if(which_opt == 2 || which_opt == 3) // STA Technology mapping tuning
        return ";strash;ifraig;dch -f;map;topo;upsize;dnsize;topo;stime;\" | grep \"Delay =\" > " + to_string(h)+"_"+to_string(i)+".result\n";
    else if(which_opt == 4 || which_opt == 5) // FPGA Technology mapping tuning
        return ";strash;ifraig;scorr;dc2;strash;dch -f;if -K 6;mfs2;lutpack -S 1;print_stats;\" | grep \"nd =\" > " + to_string(h)+"_"+to_string(i)+".result\n";
    else if(which_opt == 6 || which_opt == 7) // CNF Minimization 
        return ";strash;ifraig;write_cnf test.cnf\" | grep \"CNF stats\" > " + to_string(h)+"_"+to_string(i)+".result\n";
    else if(which_opt == 8 || which_opt == 9) // Standard-cell LIB Technology mapping tuning
        return ";strash;ifraig;dch -f;map;print_stats;\" | grep \"delay =\" > " + to_string(h)+"_"+to_string(i)+".result\n";
    else 
        return ";strash;print_stats;\" | grep \"and =\" > " + to_string(h)+"_"+to_string(i)+".result\n"; //default as AIG minimization
	/* with retiming
    if (which_opt == 0 || which_opt == 1) //AIG minimization
        return ";strash;retime;strash;ifraig;dch -f;strash;print_stats;\" | grep \"and =\" > " + to_string(h)+"_"+to_string(i)+".result\n";
    else if(which_opt == 2 || which_opt == 3) // STA Technology mapping tuning
        return ";strash;retime;strash;ifraig;dch -f;map;topo;upsize;dnsize;topo;stime;\" | grep \"Delay =\" > " + to_string(h)+"_"+to_string(i)+".result\n";
    else if(which_opt == 4 || which_opt == 5) // FPGA Technology mapping tuning
        return ";strash;ifraig;scorr;dc2;dretime;retime;strash;dch -f;if -K 6;mfs2;lutpack -S 1;print_stats;\" | grep \"nd =\" > " + to_string(h)+"_"+to_string(i)+".result\n";
    else if(which_opt == 6 || which_opt == 7) // FPGA Technology mapping tuning
        return ";strash;ifraig;strash;write_cnf test.cnf\" | grep \"CNF stats\" > " + to_string(h)+"_"+to_string(i)+".result\n";
    else if(which_opt == 8 || which_opt == 9) // FPGA Technology mapping tuning
        return ";strash;retime;strash;ifraig;dch -f;map;print_stats;\" | grep \"delay =\" > " + to_string(h)+"_"+to_string(i)+".result\n";
    else 
        return ";strash;print_stats;\" | grep \"and =\" > " + to_string(h)+"_"+to_string(i)+".result\n"; //default as AIG minimization
*/

}

// generate random constrained commands (stored in vector of strings)
vector<string> constr_random_commands(vector<string> default_opts, int repeats, int nCommands, int pos, 
        string design,int which_opt, string lib){
    
    vector<string> random_c; //final vector return
    //initialization
    vector<string> head = create_headers(pos, default_opts);
    string begin;
    if (lib!="dontCare")
    	begin="abc -c \"read " + design + ";strash;read "+lib+";";
    else
    	begin="abc -c \"read " + design + ";strash;";
    cout<<"begin:"<<begin<<endl;
    for (int h=0; h<head.size(); h++){
        for (int i=0; i<nCommands; i++){
            string command_unix = begin + head[h]+";" + constrain_random_opts(default_opts, repeats,pos);
            command_unix += abc_stats_commmands(h,i,which_opt);
            random_c.push_back(command_unix); 
        }
    }
    return random_c;
}

float get_area_from_result(string resultFile)
{
    int find_area=0;
    string input;
    ifstream fin(resultFile);
    while(getline(fin,input)){
        if (input.size()>0)
            break;
    }
    find_area = input.find("area =",0);
    int find_level = 0; find_level = input.find("delay",find_area);
    string area;
    if(find_area>0)
    {
        area.assign(input, find_area+6, find_level - find_area - 6);
        return stof(area);
    }
    else{ return 0.0; } //error message TBD 
    return 0.0;
}

//######################################**###################//
//######### Get result for STA-aware Techmap Tuning, including gate sizing ###################//
//######################################**###################//


float get_sta_delay_from_result(string resultFile)
{
    string input;
    ifstream fin(resultFile);
    while(getline(fin,input)){
        if (input.size()>0)
            break;
    }
    int find_sta_delay = input.find("Delay =",0);
    int find_level = 0; 
    string sta_delay;
    if ( find_sta_delay >0 ){
        find_level = input.find("ps",find_sta_delay);
        sta_delay.assign(input, find_sta_delay+7, find_level - find_sta_delay - 7);
        return stof(sta_delay);
    }
    else{
        return 0.0;
    }
    return 0.0;
}

float get_sta_area_from_result(string resultFile)
{
    string input;
    ifstream fin(resultFile);
    while(getline(fin,input)){
        if (input.size()>0)
            break;
    }
    int find_sta_delay = input.find("Area =",0);
    int find_level = 0; 
    string sta_delay;
    if ( find_sta_delay >0 ){
        find_level = input.find(" (",find_sta_delay);
        sta_delay.assign(input, find_sta_delay+6, find_level - find_sta_delay - 6);
        return stof(sta_delay);
    }
    else{
        return 0.0;
    }
    return 0.0;
}


//######################################**###################//
//######### Get result for AIG optimization Tuning ###################//
//######################################**###################//

float get_aig_size_from_result(string resultFile)
{
    string input;
    ifstream fin(resultFile);
    while(getline(fin,input)){
        if (input.size()>0)
            break;
    }
    int find_aig = input.find("and =", 0);
    int find_level = 0; find_level = input.find("lev",find_aig);
    string aig;
    if(find_aig>0)
    {
        aig.assign(input, find_aig+5, find_level - find_aig - 5);
        return stof(aig);
    }
    else{ return 0.0; } //error message
    return 0.0;
}

float get_aig_level_from_result(string resultFile)
{
    string input;
    ifstream fin(resultFile);
    while(getline(fin,input)){
        if (input.size()>0)
            break;
    }
    int find_lev = input.find("lev =", 0);
    string aig;
    if(find_lev>0)
    {
        aig.assign(input, find_lev+5, input.length() - find_lev - 5);
        return stof(aig);
    }
    else{ return 0.0; } //error message
    return 0.0;
}


//######################################**###################//
//######### Get result for FPGA-Mapping Tuning ###################//
//######################################**###################//
float get_FPGA_size_from_result(string resultFile)
{
    string input;
    ifstream fin(resultFile);
    while(getline(fin,input)){
        if (input.size()>0)
            break;
    }
    int find_luts = input.find("nd =", 0);
    int find_level = 0; find_level = input.find("edge",find_luts);
    string aig;
    if(find_luts>0)
    {
        aig.assign(input, find_luts+4, find_level - find_luts - 4);
        return stof(aig);
    }
    else{ return 0.0; } //error message
    return 0.0;
}

float get_FPGA_level_from_result(string resultFile)
{
    string input;
    ifstream fin(resultFile);
    while(getline(fin,input)){
        if (input.size()>0)
            break;
    }
    int find_lev = input.find("lev =", 0);
    string aig;
    if(find_lev>0)
    {
        aig.assign(input, find_lev+5, input.length() - find_lev - 5);
        return stof(aig);
    }
    else{ return 0.0; } //error message
    return 0.0;
}
//###  END   ####//


//######################################**###################//
//######### Get result for SAT-CNF Tuning ###################//
//######################################**###################//
float get_CNF_clauses_from_result(string resultFile)
{
    string input;
    ifstream fin(resultFile);
    while(getline(fin,input)){
        if (input.size()>0)
            break;
    }
    int find_clauses = input.find("Clauses =", 0);
    int find_end = 0; find_end = input.find(".",find_clauses);
    string clauses;
    if(find_clauses>0)
    {
        clauses.assign(input, find_clauses+9, find_end - find_clauses - 9);
        return stof(clauses);
    }
    else{ return 0.0; } //error message
    return 0.0;
}
float get_CNF_literals_from_result(string resultFile)
{
    string input;
    ifstream fin(resultFile);
    while(getline(fin,input)){
        if (input.size()>0)
            break;
    }
    int find_literals = input.find("Literals =", 0);
    int find_end = 0; find_end = input.find(".",find_literals);
    string literals;
    if(find_literals>0)
    {
        literals.assign(input, find_literals+10, find_end - find_literals- 10);
        return stof(literals);
    }
    else{ return 0.0; } //error message
    return 0.0;
} 
//###  END   ####//


//######################################**###################//
//######### Get result for SAT-CNF Tuning ###################//
//######################################**###################//

float get_delay_noSTA_from_result(string resultFile)
{
    string input;
    ifstream fin(resultFile);
    while(getline(fin,input)){
        if (input.size()>0)
            break;
    }
    int find_delay = input.find("delay =", 0);
    int find_end = 0; find_end = input.find("lev",find_delay);
    string delay;
    if(find_delay>0)
    {
        delay.assign(input, find_delay+7, find_end - find_delay - 7);
        return stof(delay);
    }
    else{ return 0.0; } //error message
    return 0.0;
}

float get_area_noSTA_from_result(string resultFile)
{
    string input;
    ifstream fin(resultFile);
    while(getline(fin,input)){
        if (input.size()>0)
            break;
    }
    int find_area = input.find("area =", 0);
    int find_end = 0; find_end = input.find("delay",find_area);
    string area;
    if(find_area>0)
    {
        area.assign(input, find_area+6, find_end - find_area - 6);
        return stof(area);
    }
    else{ return 0.0; } //error message
    return -1;
} 
//###  END   ####//

float get_results_universe(string resultFile, int which_opt)
{
    float res = 0.0;
    if (which_opt == 0) // aig #nodes
        res = get_aig_size_from_result(resultFile);
    else if(which_opt==1) // aig #lev
        res = get_aig_level_from_result(resultFile);
    else if (which_opt==2) //sta delay
        res = get_sta_delay_from_result(resultFile);
    else if (which_opt==3) // sta area
	res = get_sta_area_from_result(resultFile);
    else if (which_opt==4) // FPGA #lut
        res = get_FPGA_size_from_result(resultFile);
    else if (which_opt==5) //FPGA #lev
        res = get_FPGA_level_from_result(resultFile);
    else if (which_opt==6) //SAT CNF Clauses
        res = get_CNF_clauses_from_result(resultFile);
    else if (which_opt==7) //SAT CNF Literals
        res = get_CNF_literals_from_result(resultFile);
    else if (which_opt==8) // Regular techmap delay
        res = get_delay_noSTA_from_result(resultFile);
    else if(which_opt==9) // regular Techmap area
        res = get_area_noSTA_from_result(resultFile);
    else
        res = -1;
    if (res == -1)
	assert(0);
    else if (res == 0)
	return 999999999.0;
    else
	return res;	
}

vector<float> to_vec_float(vector<vector<float> > v){
    vector<float> fvec;
    for(int i=0;i<v.size();i++){
        for(int i2=0;i2<v[i].size();i2++){
            fvec.push_back(v[i][i2]);
        }
    }
    return fvec;
}

float mean_vec_float(vector<float> v){
    float x=0.0;
    for(auto i=v.begin();i!=v.end();i++){
        x+=*i;
    }
    return x/v.size();
}

float stdev_vec_float(vector<float> v){
    float sum=0.0;
    for(auto i=v.begin();i!=v.end();i++){
        sum+=*i;
    }
    float mean = sum / v.size();
    float sq_sum  = 0.0;
    for (int i=0;i<v.size();i++)
    {
        sq_sum += (v[i]-mean)*(v[i]-mean);
    }
    float stdev = sqrt(sq_sum/v.size());
    return stdev;
}

float biased_mean_vec_float(vector<float> v, float factor){
    float mean = mean_vec_float(v);
    float stdev = stdev_vec_float(v);
    return mean - factor*stdev;
}

float min_float(vector<float> v){
    return *min_element(v.begin(), v.end());
}

float max_float(vector<float> v){
    return *max_element(v.begin(), v.end());
}

float best_so_far(vector<vector<float> > v){
    vector<float> flat = to_vec_float(v);
    return *min_element(flat.begin(), flat.end());
}

float mean_so_far(vector<vector<float> > v){
    vector<float> flat = to_vec_float(v);
    float m = accumulate( flat.begin(), flat.end(), 0.0)/flat.size();
    return m;
}


template <typename T>
void logging (int iter, float best, T const& v, string logfile) 
{
    typename T::const_iterator pos;  // iterator to iterate over coll
    ofstream outfile(logfile.c_str(), std::ofstream::out | std::ofstream::app);
    outfile<<iter<<","<<best<<",";
    for (pos=v.begin(); pos!=v.end(); ++pos) {
        outfile << *pos << ", ";
    }
    outfile<<endl;
    outfile.close();
}


template <typename T>
void printvec (T const& v) 
{
    typename T::const_iterator pos;  // iterator to iterate over coll

    for (pos=v.begin(); pos!=v.end(); ++pos) {
        std::cout << *pos << ", ";
    }
    std::cout << std::endl;
}

float update_prob(vector<float> v, float global_mean){
    int count=0;
    for(auto i=0;i!=v.size();++i){
        if (v[i] - global_mean  < -0.01){
            count++;
        }
        else
            continue;
    }
    return (float)count/v.size();
}

vector<float> update_prob_global(vector<vector<float> > v, int fSoftmax){
    //get global mean
    vector<float> temp = to_vec_float(v);
    float global_mean = mean_vec_float(temp);
    float global_stdev = stdev_vec_float(temp);
    temp.clear();
    vector<float> prob;
    for (int i=0;i<v.size();i++){
#ifdef BIASED_MEAN
        prob.push_back(update_prob(v[i],global_mean - global_stdev * BIASED_MEAN_FACTOR));
#else
        prob.push_back(update_prob(v[i],global_mean ));
#endif
    }
    //for (auto p:prob){ cout<<p<<", "; } cout<<"\n";
    if(fSoftmax==1)
        softmax(prob, prob.size());
    else
        logsoftmax(prob, prob.size());
    //for (auto p:prob){ cout<<p<<", "; } cout<<"\n";
    return prob;
}

vector<float> update_prob_short_term(vector<vector<float> > v, float Tstart, int fSoftmax){
    //get global mean
    if(Tstart>=v[0].size()){
        cout<<"Warining: Short term forget time steps are greater than gobal size\n";
        return update_prob_global(v, fSoftmax);
    }
    vector<float> temp;
    for(int i=0;i<v.size();i++){
        int nforget = int(std::round(Tstart*v[i].size()));
        //cout<<"How much forgot "<<nforget<<endl;
        for(int i2=nforget;i2<v[i].size();i2++){
            temp.push_back(v[i][i2]);
        }
    }
    float global_mean = mean_vec_float(temp); 
    float global_stdev = stdev_vec_float(temp);
    temp.clear();
    vector<float> prob;
        for (int i=Tstart;i<v.size();i++){
#ifdef BIASED_MEAN
        prob.push_back(update_prob(v[i],global_mean - global_stdev * BIASED_MEAN_FACTOR));
#else
        prob.push_back(update_prob(v[i],global_mean ));
#endif
    }
    return prob;
}

/*
vector<float> update_prob_short_term_random(vector<vector<float> > v, int Tstart){
    //get global mean
    vector<float> temp = to_vec_float(v);
    float global_mean = mean_vec_float(temp); temp.clear();
    vector<float> prob;
    if(Tstart>=v.size())
        return update_prob_global(v);
    for (int i=0; i<Tstart; i++){
        if( std::rand()%2 == 0 )
            prob.push_back(update_prob(v[i],global_mean));
    }
    for (int i=Tstart;i<v.size();i++){
        prob.push_back(update_prob(v[i],global_mean));
    }
    return prob;
}
*/

/* FUNCTION:  generate random constrained commands
    vector<string> default_opts : commands that will be used in sampling(flows)
    repeats                     : each command allowed appearance
    nCommands                   : number of commands of each arm at initial step; also refers to total number of samples
                                            at each round
    pos                         : prefix position = number of commands to be exhaustive permuted (currently only 
                                            at the head of flows)
    design                      : design name (e.g., *.blif/aig)
    global_prob_area/delay      : global probability of wining rewards
    which_opt                   : switch for area/delay opt (==0 => area) (==1 ==> delay) */

vector<string> biased_constr_random_commands(vector<string> default_opts, int repeats, int nCommands, int pos, string design,
    vector<vector<float> > global_area, vector<vector<float> > global_delay, vector<int> &res_file_loc, 
    int which_opt, int fSoftmax, string lib, string output){
    vector<string> random_c; //final vector return
    //initialization
    vector<string> head = create_headers(pos, default_opts);  // head.size => number of arms
    string begin;
    begin="abc -c \"read " + design + ";strash;read "+lib+";";
 
    //generating number of samples for each arm based on the global probability (a.k.a probability matching)
    vector<int> new_sample_constr(head.size()); // each arm has its own prob/constrain
    vector<float> global_prob_area,global_prob_delay;
    #pragma omp parallel num_threads(2)
    {
    	global_prob_area =  update_prob_global(global_area, fSoftmax);  
    	global_prob_delay = update_prob_global(global_delay, fSoftmax);
    }
    float sum_prob_area = 0.0;float sum_prob_delay = 0.0;
    #pragma omp parallel num_threads(2)
    {
    	for (auto& n : global_prob_area) { sum_prob_area += n; }
    	for (auto& m : global_prob_delay) { sum_prob_delay += m; }
    }
    if (!(global_area.size() == head.size() && global_delay.size() == head.size())){
        cout<<"Error: "<<global_area.size()<< "," << global_delay.size() <<"," <<head.size()<<endl;
        assert(0);
    }
    for(int i=0;i<head.size();i++){
        new_sample_constr[i] = std::round((global_prob_area[i])*nCommands); //currently, global_prob_area = global_prob_delay, to be added for multi-obj tuning
        /*
        if(which_opt == 0 )
            new_sample_constr[i] = std::round((global_prob_area[i])*nCommands);
            //new_sample_constr[i] = (global_prob_area[i]/sum_prob_area)*nTotal; //re-use other arms
        else if(which_opt==1 || which_opt == 2)
            new_sample_constr[i] = std::round((global_prob_delay[i])*nCommands);
            //new_sample_constr[i] = (global_prob_delay[i]/sum_prob_delay)*nTotal; //re-use other arms
            // re-use => total number of samples not changed; low prob arm actions shift to high prob arms
        else
            assert(0);
        */
    }
#ifdef DETAIL_VERBOSE
    ofstream outfile(output.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
    outfile<<"*********  Info  ************\n";
    outfile<<"******  ARM   --  ACTIONS --  WINRATE  *********\n";
    for(int i=0;i<global_delay.size();i++){
        outfile<<"\t ["<<i<<"] -- ["<<global_delay[i].size()<<"] : ["<<global_prob_delay[i]<<"]"<<endl;
    }
#endif
    res_file_loc.clear();
    for (int h=0; h<head.size(); h++){
        res_file_loc.push_back(new_sample_constr[h]);
        for (int i=0; i<new_sample_constr[h]; i++){
            string command_unix = begin + head[h]+";" + constrain_random_opts(default_opts, repeats,pos);
            command_unix += abc_stats_commmands(h,i,which_opt);
            random_c.push_back(command_unix); 
        }
    }
    return random_c;
}


/* FUNCTION:  generate random constrained commands with [>> short term forget <<]
    vector<string> default_opts : commands that will be used in sampling(flows)
    repeats                     : each command allowed appearance
    nCommands                   : number of commands of each arm at initial step; also refers to total number of samples
                                            at each round
    pos                         : prefix position = number of commands to be exhaustive permuted (currently only 
                                            at the head of flows)
    design                      : design name (e.g., *.blif/aig)
    global_prob_area/delay      : global probability of wining rewards
    which_opt                   : switch for area/delay opt (==0 => area) (==1 ==> delay) */

vector<string> biased_constr_random_commands_forget(vector<string> default_opts, int repeats, int nCommands, int pos, string design,
        vector<vector<float> > global_area, vector<vector<float> > global_delay, vector<int> &res_file_loc, int which_opt, int fSoftmax, string lib){
    vector<string> random_c; //final vector return
    //initialization
    vector<string> head = create_headers(pos, default_opts);  // head.size => number of arms
    string begin;
    begin="abc -c \"read " + design + ";strash;read "+lib+";";

    //generating number of samples for each arm based on the global probability (a.k.a probability matching)
    int nTotal = head.size()*nCommands;         // calculate total number of samples at this round
    vector<int> new_sample_constr(head.size()); // each arm has its own prob/constrain
    vector<float> global_prob_area =  update_prob_short_term(global_area, SHORT_TERM_RATE, fSoftmax); 
    float sum_prob_area = 0.0; 
    for (auto& n : global_prob_area) { sum_prob_area += n; }
    vector<float> global_prob_delay = update_prob_short_term(global_delay, SHORT_TERM_RATE, fSoftmax); 
    float sum_prob_delay = 0.0;
    for (auto& n : global_prob_delay) { sum_prob_delay += n; }
    if (!(global_area.size() == head.size() && global_delay.size() == head.size())){
        cout<<"Error:"<<global_area.size()<< "," << global_delay.size() <<"," <<head.size()<<endl;
        assert(0);
    }
    for(int i=0;i<head.size();i++){
        if(which_opt==0)
            new_sample_constr[i] = (global_prob_area[i]/sum_prob_area)*nTotal;
        else if(which_opt==1 || which_opt==5)
            new_sample_constr[i] = (global_prob_delay[i]/sum_prob_delay)*nTotal;
        else
            assert(0);
    }
#ifdef DETAIL_VERBOSE
    for(int i=0;i<head.size();i++){
        cout<<"Number of samples at arm-"<<new_sample_constr[i]<<","<<global_prob_delay[i]<<","<< nTotal <<endl;
    }
#endif
    res_file_loc.clear();
    for (int h=0; h<head.size(); h++){
        res_file_loc.push_back(new_sample_constr[h]);
        for (int i=0; i<new_sample_constr[h]; i++){
            string command_unix = begin + head[h]+";" + constrain_random_opts(default_opts, repeats,pos);
            if(which_opt==5)
                command_unix += ";map;topo;upsize;dnsize;topo;stime;\" | grep \"Delay =\" > " + to_string(h)+"_"+to_string(i)+".result\n";
            else
                command_unix += ";map;print_stats;\" | grep \"delay =\" > " + to_string(h)+"_"+to_string(i)+".result\n";
            random_c.push_back(command_unix); 
        }
    }
    return random_c;
}


/* Function: get results and update the global probability distribution
    int nCommands, int repeats, int prefix_pos
    string output                        :  output result file
    vector<vector<float> > &global_area   :  area global history  
    vector<vector<float> > &global_delay  :  delay global history  */

vector<float> get_results(int nCommands, int repeats, int prefix_pos, string output, 
    vector<vector<float> > &global_area, vector<vector<float> > &global_delay, int which_opt)
{
    vector<float> res_of_this_iter;
    //int head_size = create_headers(prefix_pos, default_options()).size();
    int head_size = choose_n_k(nCommands, prefix_pos); 
    ofstream outfile(output.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
    map<int,vector<float> > prefix_res_area;
    map<int,vector<float> > prefix_res_delay;
    for (int i=0; i<head_size;i++) //i is first order result indicator; this should match "head.size" 
    {
        vector<float> res_area, res_delay;
        for (int i2=0; i2<nCommands;i2++){
            string result_file = to_string(i)+"_"+to_string(i2)+".result";
            res_area.push_back(get_results_universe(result_file,which_opt));
            res_delay.push_back(get_results_universe(result_file,which_opt));
            global_delay[i].push_back(get_results_universe(result_file,which_opt));
            global_area[i].push_back(get_results_universe(result_file,which_opt));
            res_of_this_iter.push_back(get_results_universe(result_file,which_opt));
        }
        assert(res_area.size()==res_delay.size());
#ifdef DETAIL_VERBOSE
        prefix_res_area.insert(std::pair<int,vector<float> >(i,res_area));
        prefix_res_delay.insert(std::pair<int,vector<float> >(i,res_delay));
#endif
    }
#ifdef DETAIL_VERBOSE
    vector<float> temp = to_vec_float(global_area);
    float global_mean_area = mean_vec_float(temp);
    outfile<<"(Warning)Mean : "<<global_mean_area<<"; best : "<<min_float(temp)
        <<"; size : "<<temp.size()<<"stdev: "<<stdev_vec_float(temp)<<"\n";
    for(auto i=prefix_res_area.begin();i!=prefix_res_area.end();i++)
    {
        float temp_stdev = stdev_vec_float(i->second);
        float temp_mean = mean_vec_float(i->second);
#ifdef BIASED_MEAN
        outfile<<i->first<<": mean="<<temp_mean<<"; stdev="<<temp_stdev<<"; current_epoch_win_rate : "
                << update_prob(i->second, global_mean_area - BIASED_MEAN_FACTOR * stdev_vec_float(i->second)) <<endl;
#else
        outfile<<i->first<<": mean="<<temp_mean<<"; stdev="<<temp_stdev<<"; current_epoch_win_rate : "
                << update_prob(i->second, global_mean_area) <<endl;
#endif    
    }

    /*
    temp = to_vec_float(global_delay);
    float global_mean_delay = mean_vec_float(temp);
    outfile<<"D-glob mean : "<<global_mean_delay<<"; best : "<<min_float(temp)
        <<"; size : "<<temp.size()<<"\n";
    for(auto i=prefix_res_delay.begin();i!=prefix_res_delay.end();i++)
    {
        outfile<<i->first<<": mean="<<mean_vec_float(i->second)<<"; current_epoch_win_rate : "
                << update_prob(i->second, global_mean_delay) <<endl;
    }
    */
#endif
    outfile<<"\n";
    outfile.close();
    return res_of_this_iter;
}


/* Function: get results and update the global probability distribution
    int nCommands, int repeats, int prefix_pos
    string output                        :  output result file
    vector<vector<float> > &global_area   :  area global history  
    vector<vector<float> > &global_delay  :  delay global history  
    vector<int> &res_file_loc            :  recorded the biased number of samples */ 

vector<float> get_results_biased(int nCommands, int repeats, int prefix_pos, string output, 
    vector<vector<float> > &global_area, vector<vector<float> > &global_delay, vector<int> &res_file_loc, int which_opt)
{
    vector<float> res_of_this_iter;
    int head_size = choose_n_k(nCommands, prefix_pos); 
    ofstream outfile(output.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
    map<int,vector<float> > prefix_res_area;
    map<int,vector<float> > prefix_res_delay;
    //for (int i=0; i<factorial(prefix_pos);i++) //i is first order result indicator
    for (int i=0; i<head_size;i++) //i is first order result indicator
    {
        vector<float> res_area, res_delay;
        for (int i2=0; i2<res_file_loc[i];i2++){ // the number of samples are now biased; it is recorded in res_file_loc
            string result_file = to_string(i)+"_"+to_string(i2)+".result";
            res_area.push_back(get_results_universe(result_file,which_opt));
            res_delay.push_back(get_results_universe(result_file,which_opt));
            global_delay[i].push_back(get_results_universe(result_file,which_opt));
            global_area[i].push_back(get_results_universe(result_file,which_opt));
            res_of_this_iter.push_back(get_results_universe(result_file,which_opt));
        }
#ifdef DETAIL_VERBOSE
        prefix_res_area.insert(std::pair<int,vector<float> >(i,res_area));
        prefix_res_delay.insert(std::pair<int,vector<float> >(i,res_delay));
#endif
    }
#ifdef DETAIL_VERBOSE
    vector<float> temp = to_vec_float(global_area);
    float global_mean_area = mean_vec_float(temp);
    outfile<<"Mean : "<<global_mean_area<<"; best : "<<min_float(temp)
        <<"; size : "<<temp.size()<<"stdev: "<<stdev_vec_float(temp)<<"\n";
    for(auto i=prefix_res_area.begin();i!=prefix_res_area.end();i++)
    {
        outfile<<i->first<<": mean="<<mean_vec_float(i->second)<<"; stdev="<<stdev_vec_float(i->second)<<"; current_epoch_win_rate : "
                << update_prob(i->second, global_mean_area) <<endl;
    }
    /*
    temp = to_vec_float(global_delay);
    float global_mean_delay = mean_vec_float(temp);
    outfile<<"D-glob mean : "<<global_mean_delay<<"; best : "<<min_float(temp)
        <<"; size : "<<temp.size()<<"\n";
    for(auto i=prefix_res_delay.begin();i!=prefix_res_delay.end();i++)
    {
        outfile<<i->first<<": mean="<<mean_vec_float(i->second)<<"; current_epoch_win_rate : "
                << update_prob(i->second, global_mean_delay) <<endl;
    }
    */
#endif
    outfile<<"\n";
    outfile.close();
    return res_of_this_iter;
}


/* Function: play the game and update the global distribution for next sampling and play
    int nCommands, int repeats, int prefix_pos, vector<string> default_opts, string design
    string out                          : if(verbose) print out internal results
    vector<vector<float> > &global_area  : global history of area
    vector<vector<float> > &global_delay : global history of delay
    int iter                            : current iteration of play
    int forget                          : forget/non-forget function applied switch
    int which_opt                       : area/delay switch  */
vector<float> sample(int nCommands, int repeats, int prefix_pos, vector<string> default_opts, string design, 
        string out, vector<vector<float> > &global_area, vector<vector<float> > &global_delay,
        vector<string> &global_commands,    int iter, int forget, int which_opt, int fSoftmax, string lib)
{
    //cout<<"1:|"<<global_area.size()<<","<<global_delay.size()<<endl;
    vector<int> res_file_loc;
    vector<string> commands_vec;
    if (iter==0 ) // initial with random 
        commands_vec = constr_random_commands(default_opts, repeats, nCommands, prefix_pos, design, which_opt, lib); //default, repeats, number_of_commands, prefix pos 
    else
        if(!forget)
            commands_vec = biased_constr_random_commands(default_opts, repeats, nCommands, prefix_pos, design, 
                global_area, global_delay, res_file_loc, which_opt, fSoftmax, lib, out);  //
        else
            commands_vec = biased_constr_random_commands_forget(default_opts, repeats, nCommands, prefix_pos, design, 
                global_area, global_delay, res_file_loc, which_opt,fSoftmax,lib);   //

    //cout<<"Number of Samples: "<<commands_vec.size()<<endl;
    
    int no_arm_win=0;
    if(commands_vec.size() == 0) // in case no arm win anything ; need a threthold (number of iters) of trying this
    {
         cout<<"Warning: Trying uniform random samples again since all arms wining rate is 0\n";
         commands_vec = constr_random_commands(default_opts, repeats, nCommands, prefix_pos, design, which_opt, lib);
         no_arm_win = 1;
    }

#pragma omp parallel for
    for (int i=0; i<commands_vec.size();++i)
        system(commands_vec[i].c_str());

//#pragma omp for
    for (int i=0; i<commands_vec.size();++i){
        //if(find(global_commands.begin(), global_commands.end(), commands_vec[i]) != global_commands.end() ) 
        //    assert(0);
        global_commands.push_back(commands_vec[i]);
    }

    vector<float> res_of_this_iter;
    if (iter==0 || commands_vec.size() == 0 || no_arm_win) // initial with random 
    {
        res_of_this_iter = get_results(nCommands, repeats, prefix_pos, out, global_area, global_delay, which_opt);
    }
    else{
        res_of_this_iter = get_results_biased(nCommands, repeats, prefix_pos, out, global_area, global_delay, res_file_loc, which_opt);
    }
    if(1){
        logging(iter, best_so_far(global_area), update_prob_global(global_area, fSoftmax), design+".log");
        cout <<iter<<","<< best_so_far(global_area)<<endl;
    }
    string clean_result = "rm *.result";
    //system(clean_result.c_str());
    if(!(global_commands.size() == to_vec_float(global_area).size())){
        cout<<"Error: global_cmd != results; error message "<<global_commands.size()<<","<<to_vec_float(global_area).size()<<
            ","<<to_vec_float(global_delay).size()<<endl;
        assert(0);
    }

    return res_of_this_iter;
}

void best_so_far_command(vector<string> global_commands, vector<vector<float> > global_area,
    vector<vector<float> > global_delay, int which_opt, string logfile){
    vector<float> temp;float best;
    temp = to_vec_float(global_delay);
    best = best_so_far(global_delay);
    assert(temp.size() == global_commands.size());
    int index = -1;
    for(int i=0;i<temp.size();i++)
    {
        if(temp[i] <= best){
            index = i;
            break;
        }
    }
    ofstream outfile(logfile.c_str(), std::ofstream::out | std::ofstream::app);   
    if(index != -1){
        //cout<<"One of the best command(s) found:\n"<<global_commands[index]<<endl;
        cout<<"One of the best command(s) found (Result = "<<best<<")\n"<<clean_grep(global_commands[index])<<endl;
        outfile<<"One of the best command(s) found (Result = "<<best<<")\n"<<clean_grep(global_commands[index])<<endl;
    }
    else{
        cout<<"Error\n";
        assert(0);
    }
    outfile.close();
}


void bayes_flow_tune(char* Design, int repeats, int prefix_pos, int target, int nSamples, 
                     int mab_iter, int fForget, int fSoftmax, char* LIB, char* opts) {
    string design = Design;
    string lib;
    if(target!=2 && target!=3 && target!=8 && target!=9) // if obj not related to Technology mapping
        lib = "dontCare";
    else
        lib = LIB;
    int forget = fForget;  // enable long-short-term memory for probability upate
                     // check the long-short-term rate at top of the file : global var => SHORT_TERM_RATE
    int which_opt = 0; //default(0) as AIG minimization (#nodes)
    which_opt = target;
    /*   target=0   => AIG minimization : #nodes
     *   target=1   => AIG minimization : #lev
     *   target=2   => Technology map (post-STA analysis) : sta-delay
     *   target=3   => Technology map (post-STA analysis) : sta-area // to do
     *   target=4   => FPGA mapping (if -K 6 as default) : #LUTs 
     *   target=5   => FPGA mapping (if -K 6 as default) : #lev 
     *   target=6   => SAT-CNF minimization : #Clauses
     *   target=7   => SAT-CNF minimization : #Literals
     *   target=8   => Technology map (regular map result): Delay
     *   target=9   => Technology map (regular map result): Area
    */
    string out = ".temp.result.txt"; int rm_log = system( ("rm "+out).c_str());
    vector<string> default_opts;
    if(which_opt == 0 || which_opt == 3 ||
       which_opt == 4 || which_opt == 4 ||
       //which_opt == 4 || which_opt == 6 ||
       which_opt == 7 || which_opt == 9)
    {
        default_opts = default_options();
    }
    else
        default_opts = default_options();
    // if customized opts (cmds) are provided
    if (opts != NULL){
	default_opts = split(opts, ",");
        cout<<"Load customized opts for tuning. Size: "<<default_opts.size()<<endl;
    }
    int sel_head = 1; int init_global=0; // has to be fixed later, sel_head is now manually set
    if(sel_head == 0)
        init_global = (factorial(prefix_pos));
    else
        init_global = choose_n_k(default_opts.size(), prefix_pos);
    
    int nCommands = init_global; // number of opts 
    //initialize global results vectors
    vector<vector<float> > global_delay(init_global,vector<float>(0));
    vector<vector<float> > global_area(init_global, vector<float>(0));
    vector<string>         global_commands(0); // global commands; used to give best found command at the end 
    vector<float> seq_global_delay;    
    string remove_log = "rm "+design+".log"; system(remove_log.c_str()); //remove log file if exists
    {
        vector<float> res_of_this_iter;
        for (int i=0;i<mab_iter;i++){
            res_of_this_iter = sample(nCommands, repeats, prefix_pos, default_opts, design, out, 
                                      global_area, global_delay, global_commands, i, forget, which_opt, fSoftmax, lib);
            for( int res_i = 0; res_i<res_of_this_iter.size();res_i++){
                seq_global_delay.push_back(res_of_this_iter[res_i]);
            }
        }
        //best_so_far_command(global_commands , global_area , global_delay , which_opt , design+".log");

    }
    float best_res = best_so_far(global_delay);
    string remove_result = "rm *.result";
    int rm_temp_result = system(remove_result.c_str());
    assert(seq_global_delay.size() == global_commands.size());
    ofstream outfile(design+".log", std::ofstream::out | std::ofstream::app);
    // End of tuning; printing out the best flow(s) found 
    cout<<"Best Flow(s) (#max=5):\n"; outfile<<"Best Flow(s) (#max=5):\n";
    ofstream save_for_vtr(design+".script");
    int num_recomd_cmds = 0;
    if (which_opt != 2 && which_opt != 1 && which_opt != 5
	&& which_opt !=3 && which_opt !=4	) // 2 sta-delay, 1 aig-level, 4 lut-level
    { 
	    for (int i=0;i<seq_global_delay.size();i++)
	    {
		    if(seq_global_delay[i]==best_res){
			    cout<<clean_flow_only_cmd_yosys(clean_grep(global_commands[i]))<<endl;
			    outfile<<clean_flow_only_cmd_yosys(clean_grep(global_commands[i]))<<endl;
			    //save_for_vtr<<clean_flow_only_cmd_yosys(clean_grep(global_commands[i]))<<endl;
			    save_for_vtr<<clean_grep_2(global_commands[i])<<endl;
			    num_recomd_cmds++;
			    if (num_recomd_cmds>=5)
				    break;
		    }
	    }
    }
    else{
	    for (int i=0;i<seq_global_delay.size();i++)
	    {
		    if(seq_global_delay[i]==best_res){
			    cout<<clean_flow_no_mapping(clean_grep(global_commands[i]))<<endl;
			    outfile<<clean_flow_no_mapping(clean_grep(global_commands[i]))<<endl;
			    //save_for_vtr<<clean_flow_no_mapping(clean_grep(global_commands[i]))<<endl;
			    save_for_vtr<<clean_grep_2(global_commands[i])<<endl;
			    num_recomd_cmds++;
			    if (num_recomd_cmds>=5)
				    break;
		    }
	    }
    }
    save_for_vtr.close();
    outfile.close();
    return ;
}
