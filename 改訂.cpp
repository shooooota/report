#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib> 
#include <valarray>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
using namespace std;

// zero tolerances
static const double epsilon1 = 0.00001;
static const double epsilon2 = 0.00000001;

// number of constraints
static size_t m;

// number of variables
static size_t n;

// a structure that represents a basic variable
// containing its label (ranges from 0 to m + n - 1) and its current value
struct variable {
    size_t label;
    double value;
};

// a structure that represents an eta matrix
// containing the column that's different from an identity matrix (ranges from 1 to m - 1)
// and the values in that column
struct eta {
    size_t col;
    vector<double> values;
};




bool mComparator(variable v1, variable v2) {
    return v1.value > v2.value;
}


vector<string> split(string& input, char delimiter)
{
    istringstream stream(input);
    string field;
    vector<string> result;
    while (getline(stream, field, delimiter)) {
        result.push_back(field);
    }
    return result;
}



int main(int argc, const char * argv[]) {
    
    ofstream data_file("bs2^10.csv");

    // Read in the number of constraints and
    // the number of variables
    cin >> m >> n;
    
    // Coefficients in the objective function
    double objFuncCoeff[n + m];
    
    ifstream ifs("bigdata.csv");
    string line;
    vector<double> in;
    while (getline(ifs, line)) {
        
        vector<string> strvec = split(line, ',');
        for (int i=0; i<strvec.size();i++){
            in.push_back(stod(strvec.at(i)));
            //printf("%f ", stod(strvec.at(i)));

        }
    }


    /*
    ifstream ifs2("data2.csv");
    string line2;
    double in2[30] = {43, 89, 52, 87, 32, 52, 43, 3, 52, 7, 54, 52, 98, 89, 52, 43, 89, 98, 9, 89, 98, 8, 21, 25, 43, 65, 52, 43, 6, 52};
    while (getline(ifs2, line)) {
        
        vector<string> strvec2 = split(line2, ',');
        for (int i=0; i<strvec2.size();i++){
            //in2.push_back(stod(strvec2.at(i)));
            printf("%f ", stod(strvec2.at(i)));

        }
    }
    */

    // Read in the coefficients in the objective function
    for (size_t col = 0; col < n; ++col) {
        objFuncCoeff[col] = in[col];
    }
    
    // Populate the rest of C to be 0
    for (size_t col = n; col < m + n; ++col) {
        objFuncCoeff[col] = 0.0;
    }
    
    // Matrix A represented as a 1-D array
    // with rows stacked next to each other
    double A[m * (n + m)];
    
    // Column of labels and values of the basic variables in the basic feasible solution
    variable b[m];
    
    // Column of variable (only with labels, from 0 to m + n - 1) of the nonbasic variables
    size_t nonbasic[n];

    struct ea {
        size_t col;
        double values[100] ={0.0};
    };
    int o = 10;
    //ea *S;
    //S = new ea[(1<<o)];


    //srand((unsigned int)time(NULL));

    /* bを求めている*/
    ea x[o];
    double S[100] = {}; 
    for (size_t l = 0; l < o; l++){
        double w = 0.0;
        for (size_t i = 0; i < m; i++){
            x[l].values[i] = in[l + (i + 1) * (n + o) + n];//rand()%100;
            //printf("%f ", x[l].values[i]);
        }
    }
    vector<double> kim;
    vector<double> sum;

    bool flag = true;
    for (int bit = 1; bit < (1<<o); ++bit)
    //for(int l=0; l < o; l++)
    {

        /* 
        bit で表される集合の処理を書く 
         きちんとできていることを確認してみる 
         */
        // bit の表す集合を求める
        vector<int> s;
        for (int i = 0; i < o; ++i) 
        {
            if (bit & (1<<i)) { // i が bit に入るかどうか
                s.push_back(i);
            }
        }
        for (int i = 0; i < m; i++){
            S[i] = 0.0;
        }
        // bit の表す集合の出力
        //double SUM = 0.0;
        for (int i = 0; i < (int)s.size(); ++i) 
        {
            for (size_t a = 0; a < m; a++){
                S[a] += x[s[i]].values[a];
            }
        }
/*
            }
            if((int)s.size() == 1){
                S[bit].col = s[i];
                //data_file << S[bit].col << ",";
            }else{
                S[bit].col = 11;
            }
        
        for (size_t a = 0; a < m; a++){
            SUM += S[bit].values[a];
        }

        for (size_t a = 0; a < m; a++){
            Snorm[bit].values[a] =  S[bit].values[a] / SUM;
            printf("%f %f %f\n", S[bit].values[a], Snorm[bit].values[a], SUM);
        }
*/
    



        double z = 0.0;


        //LAST: ;


            for (size_t row = 0; row < m; ++row) {
                for (size_t col = 0; col <= n; ++col) {
                    if (col == n) {
                        double bRow = S[row];
                        variable bVar = {n + row, bRow};
                        b[row] = bVar;
                    } else {
                        A[row * (m + n) + col] = in[col + (n + o)*(row + 1)];
                    }
                }
            }
        /*
        if(flag){
            // Initialize columns for decision variables in matrix A
            // Initialize b, the array of basic variables
        } else {
            for (size_t row = 0; row < m; ++row) {
                for (size_t col = 0; col <= n; ++col) {
                    if (col == n) {
                        double bRow = Snorm[bit].values[row];
                        variable bVar = {n + row, bRow};
                        b[row] = bVar;
                    } else {
                        A[row * (m + n) + col] = in[n + col + row * n];
                    }
                }
            }           
        }
        */
        
        // Initialize columns for slack variables in matrix A
        for (size_t row = 0; row < m; ++row) {
            size_t base = (m + n) * row + n;
            for (size_t col = 0; col < m; ++col) {
                if (col != row) {
                    A[base + col] = 0.0;
                } else {
                    A[base + col] = 1.0;
                }
            }
        }
        
        // Initialize the nonbasic variables' labels to be 0 through (n - 1)
        for (size_t i = 0; i < n; ++i) {
            nonbasic[i] = i;
        }
        
        // Check initial feasibility
        for (size_t row = 0; row < m; ++row) {
            if (b[row].value < 0.0) {
                return 0;
            }
        }
    
        // Initial basic solution is feasible, now proceed with the Simplex Method
        
        // A counter to remember the current iteration number
        size_t counter = 1;
        
        // An array of eta matrices representing previous pivots
        vector<eta> pivots;
        
        // Initial value of objective function
        
        while (true) {
            // compute y using eta matrices (yB = Cb)
            vector<double> y(m);
            
            // initialize y to be Cb
            for (size_t row = 0; row < m; ++row) {
                variable v = b[row];
                y[row] = objFuncCoeff[v.label];
            }
            
            // solving y in yB = Cb
            for (auto rIter = pivots.crbegin(); rIter != pivots.crend(); ++rIter) {
                eta pivot = *rIter;
                size_t colToChange = pivot.col;
                double yOriginal = y[colToChange];
                
                for (size_t row = 0; row < pivot.values.size(); ++row) {
                    if (row != colToChange) {
                        yOriginal -= pivot.values[row] * y[row];
                    }
                }
                
                double yNew = yOriginal / pivot.values[colToChange];
                y[colToChange] = yNew;
            }
            
            // choose an entering column
            // using the condition Cn > ya, where "a" is a column of An
            
            // a vector to keep track of the variables
            // whose coefficients in the objective function in this iteration are positive
            vector<variable> cnbars;
            
            size_t enteringLabel = nonbasic[0];
            double largestCoeff = -1.0;

            for (size_t i = 0; i < n; ++i) {
                size_t varLabel = nonbasic[i];
                double cni = objFuncCoeff[varLabel];
                double yai = 0.0;
                
                for (size_t yIndex = 0; yIndex < m; ++yIndex) {
                    yai += y[yIndex] * A[yIndex * (m + n) + varLabel];
                }
                
                double cnbar = cni - yai;
                
                if (cnbar > epsilon1) {
                    variable v = {varLabel, cnbar};
                    
                    cnbars.push_back(v);
                    
                    if (cnbar > largestCoeff) {
                        largestCoeff = cnbar;
                        enteringLabel = varLabel;
                    }
                }
            }

            // sort the variables into descending order
            // based on their coefficients in the objective function
            sort(cnbars.begin(), cnbars.end(), mComparator);
    

            size_t enteringVariable_index = 0;
            
            // compute the column d in Anbar
            // for the entering variable
            // using eta matrices (Bd = a)
            vector<double> d(m);
            
            size_t leavingLabel;
            size_t leavingRow;
            double smallest_t;
    

            while (true) {
                
                leavingLabel = -1;
                leavingRow = -1;
                smallest_t = -1;
                
                if (enteringVariable_index < cnbars.size()) {
                    enteringLabel = cnbars[enteringVariable_index].label;
                } else {
                        //kim.push_back(z);
                        //printf("%f \n", kim[bit-1]);
                        //flag = !flag;   
                        cout << z << endl;           
                    /*
                        data_file << z << "," << " "<< ",";
                        for (auto iter = y.cbegin(); iter != y.cend(); ++iter) {
                            data_file << *iter << ",";
                        }
                        data_file << " "<< ",";
                        for (int i = 0; i < m; i++){
                            data_file << S[i] << ",";
                        }
                        data_file << std::endl;
                        if (bit % 100==0){
                            data_file.close();
                            ofstream data_file("bs2^10.csv", ios::app);
                        }
                        goto PAST;
                    if(flag){
                        sum.push_back(z);
                        data_file << sum[bit-1] << ",";
                        flag = !flag;                       
                        goto PAST;
                    }else{
                    }
                    */
                }
                
                // initialize d to be the entering column a
                for (size_t row = 0; row < m; ++row) {
                    d[row] = A[row * (m + n) + enteringLabel];
                }
                
                // Go through eta matrices from pivot 1 to pivot k
                for (auto iter = pivots.cbegin(); iter != pivots.cend(); ++iter) {
                    eta pivot = *iter;
                    size_t rowToChange = pivot.col;
                    double dOriginal = d[rowToChange];
                    
                    d[rowToChange] = dOriginal / pivot.values[rowToChange];
                    
                    for (size_t row = 0; row < d.size(); ++row) {
                        if (row != rowToChange) {
                            d[row] = d[row] - pivot.values[row] * d[rowToChange];
                        }
                    }
                }
                
                // compute t for each b[i].value / d[i]
                // where d[i] > 0
                // choose the corresponding i for the smallest ratio
                // as the leaving variable
                
                // initialize smallest_t to be the first ratio where
                // the coefficient of the entering variable in that row is negative
                for (size_t row = 0; row < d.size(); ++row) {
                    if (d[row] > 0.0) {
                        leavingLabel = b[row].label;
                        leavingRow = row;
                        smallest_t = b[row].value / d[row];
                    }
                }
                if (leavingLabel == -1) {

                        //kim.push_back(z);
                        //printf("%f\n", kim[bit-1]);
                        //data_file << kim[bit-1] << ",";
                        //data_file<< std::endl;
                        //flag = !flag;
                        goto PAST;
                    /*
                    if(flag){
                        sum.push_back(z);
                        //data_file << sum[bit-1] << ",";
                        flag = !flag;
                        goto PAST;
                    }else{
                    }
                    */
                }
                
                // there is at least one ratio computed, print out the ratio(s)
                // and choose the row corresponding to the smallest ratio to leave
                
                for (size_t row = 0; row < d.size(); ++row) {
                    if (d[row] < 0.0) {
                        continue;
                    }
                    
                    double t_row = b[row].value / d[row];
                    
                    if (t_row < smallest_t) {
                        leavingLabel = b[row].label;
                        leavingRow = row;
                        smallest_t = t_row;
                    }
                }

                // check the diagonal element in the eta column
                // to see if the current choice of entering variable has to be rejected
                if (d[leavingRow] > epsilon2) {
                    break;
                } else {
                    enteringVariable_index++;
                    continue;
                }
            }
        
            // At this point we have a pair of entering and leaving variables
            // so that the entering variable is positive and the diagonal entry in the eta column
            // of the eta matrix is fairly far from zero
            
            // set the value of the entering varaible at t
            // modify b (change leaving variable to entering variable, change values of other basic vars)
            variable enteringVar = {enteringLabel, smallest_t};
            b[leavingRow] = enteringVar;
            
            for (size_t row = 0; row < sizeof(b) / sizeof(b[0]); ++row) {
                if (row != leavingRow) {
                    b[row].value -= d[row] * smallest_t;
                }
            }
            
            // push a new eta matrix onto the vector
            eta pivot = {leavingRow, d};
            pivots.push_back(pivot);
    
            nonbasic[enteringLabel] = leavingLabel;

            // increase the value of the objective function
            double increasedValue =largestCoeff * smallest_t;
    

            double originalZ = z;
            
            z += increasedValue;

            counter++;
        } 


        
        PAST: ;
    }
  
    data_file.close();
    //delete[] S;
    return 0;
}
