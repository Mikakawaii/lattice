#include <NTL/ZZ.h>
#include <NTL/vec_ZZ.h>
#include <NTL/mat_ZZ.h>
#include <time.h>
#include <cmath>
#include<random>

NTL_CLIENT

ZZ assistant_algorithm(ZZ a, ZZ q);//the round-off of a/q
ZZ my_mod(ZZ a, ZZ q);//the new mod
vec_ZZ SecretKeygen(ZZ q, long n);//SecretKeygen(1")
vec_ZZ gets_(vec_ZZ s, long n);//get s'
mat_ZZ PublicKeygen(vec_ZZ s_, ZZ q, long n);//PublicKeygen(s)
vec_ZZ Enc(mat_ZZ A, ZZ q, long n, int msg);//Enc(pk, m)
ZZ Dec(vec_ZZ c, vec_ZZ s, ZZ q);//Dec(sk, c)
vec_ZZ Noisegen(long N);//noise(m)

int main(void)
{

        long n;//the length n
        int msg;//the original message
        ZZ q, m;//the mod q and the decoded message m
        q = 15199;
        n = 300;
        vec_ZZ s, c;//the sk -> s and the ciphertext ->c
        mat_ZZ A;//the pk -> A
        /*cout << "Please enter the number q : ";//模数q
        cin >> q;//get q
       cout << "\nPlease enter the number n : ";//矩阵的列
        cin >> n;*///get n
        s = SecretKeygen(q, n);//SecretKeygen(1") //生成密钥 s
        A = PublicKeygen(gets_(s, n), q, n);//PublicKeygen(s)
        cout << "\nPlease enter the message : ";
        cin >> msg;//get the original message
        c = Enc(A, q, n, msg);//Enc(pk, m)
       // cout << "\nc = " << c;
        m = Dec(c, s, q);//Dec(sk, c)
        cout << "\n\nThe decoded message is : " << m;

    return 0;
}

ZZ my_mod(ZZ a, ZZ q)//the new mod
{
    return a - q * assistant_algorithm(a, q);
}

ZZ assistant_algorithm(ZZ a, ZZ q)//the round-off of a/q 模q
{
    ZZ tmp;
    tmp = a * 10 / q;
    if (tmp % 10 >= 5)
    {
        return a / q + 1;
    }
    else
    {
        return a / q;
    }
}

vec_ZZ SecretKeygen(ZZ q, long n)//SecretKeygen(1")
{
    vec_ZZ s;
    int i;
    srand(time(NULL));//seed
    s.SetLength(n+1);//set the length of the vector s
    s=Noisegen(n+1);
    s(n+1) = -1;
    return s;//return s -> sk
}

vec_ZZ gets_(vec_ZZ s, long n)//get s'
{
    vec_ZZ s_;
    int i;
    s_.SetLength(n);//set the length of the vector s'
    for (i = 1; i <= n; i++)
    {
        s_(i) = s(i);//assign to vector s'
        // cout<<s(i)<<" ";
    }

    return s_;
}

mat_ZZ PublicKeygen(vec_ZZ s_, ZZ q, long n)//PublicKeygen(s)
{
    long N = 2 * n * ceil(log(q));
    int i, j;
    mat_ZZ A_, A;
    vec_ZZ e, b;
    A_.SetDims(n, N);//set the rank of the matrix A'
    A.SetDims(n+1,N);//set the rank of the matrix A
    e.SetLength(N);//set the length of the vector e
    //int x=0,y=0;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < N; j++)
        {
            A_[i][j] = my_mod(to_ZZ(rand() + q), q);
            A[i][j]=A_[i][j];//assign to the matrix A'
            // assign to the vector e
            //cout<< A_[i][j]<<" ";
        }
        e=Noisegen(N);
    }
    b = s_ * A_ + e;
    for (j = 0; j < N ; j++)
    {
        A[n][j] = my_mod(b[j],q);
        //cout<<my_mod(b[j],q)<<" ";
    }
    //cout<<endl;
    return A;//return A -> pk
}

vec_ZZ Enc(mat_ZZ A, ZZ q, long n, int msg)//Enc(pk, m)
{
    long N = 2 * n * ceil(log(q));
    int i;
    vec_ZZ m, r, c, tmp1, tmp3;
    mat_ZZ tmp2;
    m.SetLength(n + 1);//set the length of the vector m
    r.SetLength(N);//set the length of the vector r
    m(n+1) = msg;
    for (i = 1; i <= n ; i++)//assign to the vector m
    {
        m(i) = 0;

    }
    //cout<<endl;
    for (i = 1; i <= N; i++)//assign to the vector r
    {
        r(i) = abs(rand()%2);
        //r(i)=my_mod(to_ZZ(rand() + q), q);
        //cout<<r(i)<<" ";
    }
    //c = my_mod(q / 2 * m + transpose(A) * r, q);
    tmp1 = (q / 2) * m;
    tmp2 = A;
    tmp3 = tmp2 * r;
    c = tmp1 + tmp3;
    for (i = 1; i <= n + 1; i++)
    {
        c(i) = my_mod(c(i), q);
    }
    return c;//return c -> ciphertext
}

ZZ Dec(vec_ZZ c, vec_ZZ s, ZZ q)//Dec(sk, c)
{
    ZZ tmp = my_mod(c * s, q);
    tmp=abs(tmp);
    if (tmp > (q / 4))
        return to_ZZ(1);
    else return to_ZZ(0);
}
vec_ZZ Noisegen(long N)
{

    default_random_engine random(time(NULL))  ; //引擎
    normal_distribution<double> n(0, 3.2); //均值, 方差
    vec_ZZ e;
    ZZ sum;
    sum=0;
    e.SetLength(N);
    for(int i=0; i < N; ++i) {
        e[i] = n(random); //取整-最近的整数
    }
    return e;
}