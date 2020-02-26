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
static const float epsilon1 = 0.1;
static const float epsilon2 = 0.001;

//  static const float epsilon1 = 0.1;
// static const float epsilon2 = 0.01;

// number of constraints
static size_t m;

// number of variables
static size_t n;
float varValues[6];
// a structure that represents a basic variable
// containing its label (ranges from 0 to m + n - 1) and its current value
struct variable
{
    size_t label;
    float value;
};

// a structure that represents an eta matrix
// containing the column that's different from an identity matrix (ranges from 1 to m - 1)
// and the values in that column
struct eta
{
    size_t col;
    vector<float> values;
};

bool mComparator(variable v1, variable v2)
{
    return v1.value > v2.value;
}

vector<string> split(string &input, char delimiter)
{
    istringstream stream(input);
    string field;
    vector<string> result;
    while (getline(stream, field, delimiter))
    {
        result.push_back(field);
    }
    return result;
}

struct ea
{
    size_t col;
    int **values;
    //float values[100] = {};
};

struct eax
{
    size_t col;
    int values[3] = {};
};

int o = 3;
//struct ea* S = new ea[400];
//ea S;
int main(int argc, const char *argv[])
{
    /*
    int** ea_value; 
    ea_value = new int* [400];
    for(int i=0; i<400; i++){
        ea_value[i] = new int [100];
    }
    // 最後に
    
    */

    //FILE *data_file;

    //for(int i=0; i<400; i++)
    //    ea[i].values = new float[100];

    ofstream data_file("ans_exam.csv");

    // Read in the number of constraints and
    // the number of variables
    cin >> m >> n;
    // Coefficients in the objective function
    float objFuncCoeff[n + m];

    ifstream ifs("exam_param.csv");
    string line, line2;
    vector<float> in;

    ifstream s("exam_param_b.csv");
    float S[3];
    while (getline(ifs, line))
    {

        vector<string> strvec = split(line, ',');
        for (int i = 0; i < strvec.size(); i++)
        {
            in.push_back(stod(strvec.at(i)));
        }
    }

    // Read in the coefficients in the objective function
    for (size_t col = 0; col < n; ++col)
    {
        objFuncCoeff[col] = in[col];
    }

    // Populate the rest of C to be 0
    for (size_t col = n; col < m + n; ++col)
    {
        objFuncCoeff[col] = 0.0;
    }

    // Matrix A represented as a 1-D array
    // with rows stacked next to each other
    float A[m * (n + m)];

    // Column of labels and values of the basic variables in the basic feasible solution
    variable b[m];

    // Column of variable (only with labels, from 0 to m + n - 1) of the nonbasic variables
    size_t nonbasic[n];

    //S.values =  ea_value;

    float z;
    for (int bit = 1; bit < (1 << o); ++bit)
    {
        z = 0.0;
        getline(s, line2);

        vector<string> st = split(line2, ',');
        /*
        */
        for (int i = 0; i < st.size(); i++)
        {
            S[i] = stoi(st.at(i));
        }

        for (size_t row = 0; row < m; ++row)
        {
            for (size_t col = 0; col <= n; ++col)
            {
                if (col == n)
                {
                    float bRow;
                    bRow = S[row];
                    /*
                        if (bit > 400 && bit < 801){
                            bRow = S.values[bit-401][row]; 
                        }else if(bit > 800){
                            bRow = S.values[bit-801][row];
                        }else{
                        }
                        if (bit > 400 && bit < 801){
                            bRow = S[bit-401].values[row]; 
                        }else if(bit > 800){
                            bRow = S[bit-801].values[row];
                        }else{
                            bRow = S[bit-1].values[row];
                        }
                        */
                    variable bVar = {n + row, bRow};
                    b[row] = bVar;
                }
                else
                {
                    A[row * (m + n) + col] = in[col + n * (row + 1)];
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
                        float bRow = Snorm[bit].values[row];
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
        for (size_t row = 0; row < m; ++row)
        {
            size_t base = (m + n) * row + n;
            for (size_t col = 0; col < m; ++col)
            {
                if (col != row)
                {
                    A[base + col] = 0.0;
                }
                else
                {
                    A[base + col] = 1.0;
                }
            }
        }

        // Initialize the nonbasic variables' labels to be 0 through (n - 1)
        for (size_t i = 0; i < n; ++i)
        {
            nonbasic[i] = i;
        }

        // Check initial feasibility
        for (size_t row = 0; row < m; ++row)
        {
            if (b[row].value < 0.0)
            {
                return 0;
            }
        }

        // Initial basic solution is feasible, now proceed with the Simplex Method

        // A counter to remember the current iteration number
        size_t counter = 1;

        // An array of eta matrices representing previous pivots
        vector<eta> pivots;

        // Initial value of objective function

        while (true)
        {
            // compute y using eta matrices (yB = Cb)
            vector<float> y(m);

            // initialize y to be Cb
            for (size_t row = 0; row < m; ++row)
            {
                variable v = b[row];
                y[row] = objFuncCoeff[v.label];
            }

            // solving y in yB = Cb
            for (auto rIter = pivots.crbegin(); rIter != pivots.crend(); ++rIter)
            {
                eta pivot = *rIter;
                size_t colToChange = pivot.col;
                float yOriginal = y[colToChange];

                for (size_t row = 0; row < pivot.values.size(); ++row)
                {
                    if (row != colToChange)
                    {
                        yOriginal -= pivot.values[row] * y[row];
                    }
                }

                float yNew = yOriginal / pivot.values[colToChange];
                y[colToChange] = yNew;
            }

            // choose an entering column
            // using the condition Cn > ya, where "a" is a column of An

            // a vector to keep track of the variables
            // whose coefficients in the objective function in this iteration are positive
            vector<variable> cnbars;

            size_t enteringLabel = nonbasic[0];
            float largestCoeff = -1.0;

            for (size_t i = 0; i < n; ++i)
            {
                size_t varLabel = nonbasic[i];
                float cni = objFuncCoeff[varLabel];
                float yai = 0.0;

                for (size_t yIndex = 0; yIndex < m; ++yIndex)
                {
                    yai += y[yIndex] * A[yIndex * (m + n) + varLabel];
                }

                float cnbar = cni - yai;

                if (cnbar > epsilon1)
                {
                    variable v = {varLabel, cnbar};

                    cnbars.push_back(v);

                    if (cnbar > largestCoeff)
                    {
                        largestCoeff = cnbar;
                        enteringLabel = varLabel;
                    }
                }
            }

            // sort the variables into descending order
            // based on their coefficients in the objective function
            sort(cnbars.begin(), cnbars.end(), mComparator);

            size_t enteringVariable_index = 0;

            // if (cnbars.size() == 0)
            // {
            //     goto PAST;
            // }

            // compute the column d in Anbar
            // for the entering variable
            // using eta matrices (Bd = a)
            vector<float> d(m);

            size_t leavingLabel;
            size_t leavingRow;
            float smallest_t;

            while (true)
            {

                leavingLabel = -1;
                leavingRow = -1;
                smallest_t = -1;

                if (enteringVariable_index < cnbars.size())
                {
                    enteringLabel = cnbars[enteringVariable_index].label;
                }
                else
                {
                    data_file << z << ", ";
                    for (size_t row = 0; row < m; ++row)
                    {
                        varValues[b[row].label] = b[row].value;
                    }
                    for (size_t col = 0; col < n; ++col)
                    {
                        varValues[nonbasic[col]] = 0.0;
                    }
                    for (size_t row = 0; row < n; ++row)
                    {
                        data_file << varValues[row] << ",";
                    }
                    data_file << "\n";

                    /*
                        if (bit % 400 == 0){
                            for(int i=0; i<400; i++){
                                delete [] ea_value[i];
                            }
                            delete [] ea_value;
                            int** ea_value; 
                            ea_value = new int* [400];
                            for(int i=0; i<400; i++){
                                ea_value[i] = new int [100];
                            }
                            S.values =  ea_value;
                        }
                            data_file.close();
                            ofstream data_file("test.csv",ios::app);
                        cout << endl;
                        if (bit >= 399){
                            cout << bit << "\t";    
                            int temporary_a;
                            cin >> temporary_a; 
                            if(temporary_a == 1){
                                 for (int i = 0; i < m; i++){
                                     cout << S[bit-1].values[i] << "\t";
                                }
                            }
                        }
                        for (int i = 0; i < m; i++){
                            cout << S[bit-1].values[i] << ",";
                        }
                        cout << endl;
                        
                        */
                    //flag = !flag;
                    goto PAST;
                    /*
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
                for (size_t row = 0; row < m; ++row)
                {
                    d[row] = A[row * (m + n) + enteringLabel];
                }

                // Go through eta matrices from pivot 1 to pivot k
                for (auto iter = pivots.cbegin(); iter != pivots.cend(); ++iter)
                {
                    eta pivot = *iter;
                    size_t rowToChange = pivot.col;
                    float dOriginal = d[rowToChange];

                    d[rowToChange] = dOriginal / pivot.values[rowToChange];

                    for (size_t row = 0; row < d.size(); ++row)
                    {
                        if (row != rowToChange)
                        {
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
                for (size_t row = 0; row < d.size(); ++row)
                {
                    if (d[row] > 0.0)
                    {
                        leavingLabel = b[row].label;
                        leavingRow = row;
                        smallest_t = b[row].value / d[row];
                    }
                }
                if (leavingLabel == -1)
                {
                    data_file << z << ", ";
                    for (size_t row = 0; row < m; ++row)
                    {
                        varValues[b[row].label] = b[row].value;
                    }
                    for (size_t col = 0; col < n; ++col)
                    {
                        varValues[nonbasic[col]] = 0.0;
                    }
                    for (size_t row = 0; row < n; ++row)
                    {
                        data_file << varValues[row] << ",";
                    }
                    data_file << "\n";

                    goto PAST;
                    /*
                    if (bit > 400 && bit < 801){
                        for (int i = 0; i < m; i++){
                            cout << S.values[bit-401][i] << ",";
                        }
                    }else if(bit > 800){
                        for (int i = 0; i < m; i++){
                            cout << S.values[bit-801][i] << ",";
                        }
                    }else{
                        for (int i = 0; i < m; i++){
                            cout << S.values[bit-1][i] << ",";
                        }
                    }
                    cout << endl;                           
                    if (bit > 400 && bit < 801){
                        for (int i = 0; i < m; i++){
                            data_file << S.values[bit-401][i] << ",";
                        }
                    }else if(bit > 800){
                        for (int i = 0; i < m; i++){
                            data_file << S.values[bit-801][i] << ",";
                        }
                    }else{
                        for (int i = 0; i < m; i++){
                            data_file << S.values[bit-1][i] << ",";
                        }
                    }
                    data_file << "\n"; 
                    if (bit % 400 == 0){
                        for(int i=0; i<400; i++){
                                delete [] ea_value[i];
                        }
                        delete [] ea_value;
                        int** ea_value; 
                        ea_value = new int* [400];
                        for(int i=0; i<400; i++){
                            ea_value[i] = new int [100];
                        }
                        S.values =  ea_value;
                    }
                        //kim.push_back(z);
                        //printf("%f\n", kim[bit-1]);
                        //data_file << kim[bit-1] << ",";
                        //data_file<< std::endl;
                        //flag = !flag;
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

                for (size_t row = 0; row < d.size(); ++row)
                {
                    if (d[row] < 0.0)
                    {
                        continue;
                    }

                    float t_row = b[row].value / d[row];

                    if (t_row < smallest_t)
                    {
                        leavingLabel = b[row].label;
                        leavingRow = row;
                        smallest_t = t_row;
                    }
                }

                // check the diagonal element in the eta column
                // to see if the current choice of entering variable has to be rejected
                if ((float)d[leavingRow] > epsilon2)
                {
                    break;
                }
                else
                {
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

            for (size_t row = 0; row < sizeof(b) / sizeof(b[0]); ++row)
            {
                if (row != leavingRow)
                {
                    b[row].value -= d[row] * smallest_t;
                }
            }

            // push a new eta matrix onto the vector
            eta pivot = {leavingRow, d};
            pivots.push_back(pivot);

            nonbasic[enteringLabel] = leavingLabel;

            // increase the value of the objective function
            float increasedValue = largestCoeff * smallest_t;

            float originalZ = z;

            z += increasedValue;

            counter++;
        }

    PAST:;
    }
    data_file.close();
    //delete [] S;
    /*
    for(int i=0; i<400; i++){
        delete [] ea_value[i];
    }
    delete [] ea_value;
    */
    return 0;
}
