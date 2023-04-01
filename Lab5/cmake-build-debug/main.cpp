#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <tuple>
#include <future>

using namespace std;

#define grad1 100
#define grad2 100

std::mutex m;

void print_vector(vector<pair<int, int>> a);

void read_polinomials(vector<pair<int, int>>& poli1, vector<pair<int, int>>& poli2){

}

vector<pair<int, int>> sequencial_regular_multiplication(vector<pair<int, int>> poli1, vector<pair<int, int>> poli2){
    //Înumlțește cele două polinaome
    vector<pair<int,int>> sum;
    for(int i = 0; i < poli1.size(); i++){
        for(int j = 0; j < poli2.size(); j++){
            sum.push_back({get<0>(poli1.at(i)) * get<0>(poli2.at(j)), get<1>(poli1.at(i)) + get<1>(poli2.at(j))});
        }
    }

    for(int i = 0; i < sum.size()-1; i++){
        for(int j = i+1; j < sum.size(); j++){
            if(sum.at(i).second == sum.at(j).second){
                sum.at(i).first += sum.at(j).first;
                sum.erase(sum.begin()+j);
                //j--;
            }
        }

    }

    return sum;
}

void multiply(int x, vector<pair<int, int>> poli, int* sum){
    for( int i = 0; i < grad2+1; i++){
        m.lock();
        *sum += x * get<0>(poli.at(i));
        m.unlock();
    }
}

void parallelized_regular_multiplication(){
    int thread_list_length = grad1 + 1;
    thread thread_list[thread_list_length];
    vector<pair<int, int>> poli1, poli2;
    read_polinomials(poli1, poli2);

    int sum = 0;
    for(int i = 0; i < grad1 + 1; i++){
        thread_list[i] = thread(multiply, get<0>(poli1.at(1)), poli2, &sum);
    }

    for(int i = 0; i < grad1 + 1; i++){
        thread_list[i].join();
    }

    cout << sum;
}

void print_vector(vector<pair<int, int>> a){
    for(tuple<int, int> x: a){
        cout <<"( "<<get<0>(x) << ", "<<get<1>(x)<<" )";
    }
    cout<<endl;
}

void add_poli(vector<pair<int, int>>& a_plus_b, vector<pair<int, int>> a) {
    bool ok = false;
    int i = 0, j = 0;
    if(a_plus_b.empty()){
        for(i = 0; i < a.size(); i++){
            a_plus_b.push_back(a[i]);
        }
        return;
    }
    while (!ok) {
        //a s-a golit, adaug tot ce este in b
        if (i > a_plus_b.size()) {
            if (j < a.size()) {
                for (int k = j; k < a.size(); k++) {
                    a_plus_b.push_back(a[k]);
                }
                ok = true;
            }
        }else if (a_plus_b[i].second > a[j].second) {
            i++;
        } else if (a_plus_b[i].second < a[j].second) {
            a_plus_b.insert(a_plus_b.begin()+i, a[j]);
            j++;
        } else {
            a_plus_b[i].first += a[j].first ;
            i++;
            j++;
        }
        if (j == a.size() && i == a_plus_b.size()) {
            ok = true;
        }
    }
}

void substract_poli(vector<pair<int, int>>& poli1, vector<pair<int, int>> poli2){
    bool ok = false;
    int i=0;
    int j=0;
    while (!ok){
        //pol1 s-a golit, adaug tot ce este in poli2
        if(i > poli1.size()){
            if(j < poli2.size()){
                for(int k = j; k < poli2.size(); k++){
                    poli1.push_back({-poli2[k].first, poli2[k].second});
                }
                ok = true;
            }
        }//poli2 s-a golit
        else if (j > poli2.size()){
            ok = true;
        }else if(poli1[i].second > poli2[j].second){
            i++;
        }else if(poli1[i].second < poli2[j].second){
            poli1.insert(poli1.begin() + i, poli2[j]);
            j++;
            i++;
        }else{
            poli1[i].first -= poli2[j].first;
            j++;
            i++;
        }
        if(i == poli1.size() && j == poli2.size()){
            ok = true;
        }
    }
}

vector<pair<int, int>> sequencial_karatsuba(vector<pair<int, int>> poli1, vector<pair<int, int>> poli2){
    //polinom de forma : {(coeficient1, exponent1), (coeficient2, exponent2), ...}
    unsigned int n = max(poli1.size(), poli2.size());

    if ( n == 1 ){
        vector<pair<int, int>> res;
        int a, b;

        a = get<0>(poli1[0]);
        b = get<0>(poli2[0]);

        res.push_back(pair<int,int>(a*b, 0));
        return res;
    }
    if(n < 10){
        return sequencial_regular_multiplication(poli1, poli2);
    }
    vector<pair<int, int>> a, b, c, d;

    for(int i = 0; i < poli1.size()/2; i++) {
        a.push_back(poli1[i]);
    }

    int x = a[a.size()-1].second;
    for(int i = a.size()-1; i >= 0; i--){
        a[i].second -= x;
    }

    for(int i = poli1.size()/2; i < poli1.size(); i++) {
        b.push_back(poli1[i]);
    }

    for(int i = 0; i < poli2.size()/2; i++){
        c.push_back(poli2[i]);
    }

    for(int i = c.size()-1; i >= 0; i--){
        c[i].second -= x;
    }

    for(int i =  poli2.size()/2; i < poli2.size(); i++){
        d.push_back(poli2[i]);
    }

    vector<pair<int, int>> ac, bd, a_plus_b, c_plus_d, a_bc_d;

    ac = sequencial_karatsuba(a, c);

    bd = sequencial_karatsuba(b, d);

    add_poli(a_plus_b, a);
    add_poli(a_plus_b, b);

    add_poli(c_plus_d, c);
    add_poli(c_plus_d, d);

    a_bc_d = sequencial_karatsuba(a_plus_b, c_plus_d);

    substract_poli(a_bc_d, ac);
    substract_poli(a_bc_d, bd);

    vector<pair<int, int>> p1p2;

    for(auto& i : ac){
        i.second += 2*x;
    }
    for(auto& i : a_bc_d){
        i.second += x;
    }

    add_poli(p1p2, ac);
    add_poli(p1p2, a_bc_d);
    add_poli(p1p2, bd);

    return p1p2;
}

void parallel_karatsuba(vector<pair<int, int>> poli1, vector<pair<int, int>> poli2, vector<pair<int, int>>& ret){
    unsigned int n = max(poli1.size(), poli2.size());

    if ( n == 1 ){
        vector<pair<int, int>> res;
        int a, b;

        a = get<0>(poli1[0]);
        b = get<0>(poli2[0]);

        ret.emplace_back(pair<int,int>(a*b, 0));
        return;
    }
    if(n <= (grad1+grad2)/4){
        ret = sequencial_regular_multiplication(poli1, poli2);
        return;
    }
    vector<pair<int, int>> a, b, c, d;

    for(int i = 0; i < poli1.size()/2; i++) {
        a.push_back(poli1[i]);
    }

    int x = a[a.size()-1].second;
    for(int i = a.size()-1; i >= 0; i--){
        a[i].second -= x;
    }

    for(int i = poli1.size()/2; i < poli1.size(); i++) {
        b.push_back(poli1[i]);
    }

    for(int i = 0; i < poli2.size()/2; i++){
        c.push_back(poli2[i]);
    }

    for(int i = c.size()-1; i >= 0; i--){
        c[i].second -= x;
    }

    for(int i =  poli2.size()/2; i < poli2.size(); i++){
        d.push_back(poli2[i]);
    }

    vector<pair<int, int>> ac, bd, a_plus_b, c_plus_d, a_bc_d;
    thread t[3];

    t[0] = thread(parallel_karatsuba, a, c, std::ref(ac));
    t[1] = thread(parallel_karatsuba, b, d, std::ref(bd));
    t[0].join();
    t[1].join();

    /*if(no_threads > 1){
        no_threads -= 2;


    }else{
        parallel_karatsuba(a, c, ac);
        parallel_karatsuba(b, d, bd);
    }*/

    add_poli(a_plus_b, a);
    add_poli(a_plus_b, b);

    add_poli(c_plus_d, c);
    add_poli(c_plus_d, d);

    t[2] = thread(parallel_karatsuba, a_plus_b, c_plus_d, std::ref(a_bc_d));
    t[2].join();

    /*if(no_threads > 0){
        no_threads --;


    }else{
        parallel_karatsuba(a_plus_b, c_plus_d, a_bc_d);
    }*/

    substract_poli(a_bc_d, ac);
    substract_poli(a_bc_d, bd);

    for(auto& i : ac){
        i.second += 2*x;
    }
    for(auto& i : a_bc_d){
        i.second += x;
    }

    add_poli(ret, ac);
    add_poli(ret, a_bc_d);
    add_poli(ret, bd);


}

int main() {
    //sequencial_regular_multiplication();
    //parallelized_regular_multiplication();
    vector<pair<int, int>> poli1, poli2, res;
/*
    read_polinomials(poli1, poli2);
*/
    for(int i = 100; i >= 0; i--){
        poli1.emplace_back(1,i);
    }
    for(int i = 100; i >= 0; i--){
        poli2.emplace_back(1,i);
    }
    /*cout << "Polinom 1:";
    int opt;
    for(int i = grad1; i >= 0; i--){
        cin >> opt;
        poli1.emplace_back(opt, i);
    }
    cout << "Polinom 2:";
    for(int i = grad2; i >= 0 ; i--){
        cin >> opt;
        poli2.emplace_back(opt, i);
    }*/
    //print_vector(sequencial_regular_multiplication(poli1, poli2));
    //print_vector(sequencial_karatsuba(poli1, poli2));
    parallel_karatsuba(poli1, poli2, res);
    print_vector(res);
    /*parallel_karatsuba(poli1, poli2, res);
    print_vector(res);*/
    return 0;
}
