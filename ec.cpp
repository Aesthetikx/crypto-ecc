#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <gmpxx.h>
#include <utility>
#include "ec_ops.h"
using namespace std;

Zp Zp::inverse() const{
        mpz_class a = this->value;
        mpz_class n = PRIME;

        mpz_class t = 0;
        mpz_class new_t = 1;
        mpz_class r = n;
        mpz_class new_r = a;

        while (new_r != 0)
        {
                mpz_class quotient = r / new_r;

                mpz_class tmp_t = t;
                t = new_t;
                new_t = tmp_t - quotient * new_t;

                mpz_class tmp_r = r;
                r = new_r;
                new_r = tmp_r - quotient * new_r;
        }

        if (t < 0)
        {
                t = t + n;
        }

        return Zp(t);
}


ECpoint ECpoint::operator + (const ECpoint &a) const {
        // Implement  elliptic curve addition
        ECpoint p = *this;
        ECpoint q = a;
        ECpoint r = new ECpoint();

        if (p.infinityPoint)
        {
                return q;
        }
        else if (q.infinityPoint)
        {
                return p;
        }

        if (!(p == q) && !(p.x == q.x))
        {
                Zp delta = (q.y - p.y) * ((q.x - p.x).inverse());
                r.x = delta * delta - p.x - q.x;
                r.y = delta * (p.x - r.x) - p.y;
        }
        else if ((p == q) && !(p.y * 2 == 0))
        {
                Zp delta = (p.x * p.x * 3 - A) * ((p.y * 2).inverse());
                r.x = delta * delta - (p.x * 2);
                r.y = delta * (p.x - r.x) - p.y;
        }
        else
        {
                r.infinityPoint = true;
        }
        return r;
}


ECpoint ECpoint::repeatSum(ECpoint p, mpz_class v) const {
        //Find the sum of p+p+...+p (vtimes)
        if (v == 0)
        {
                return 0;
        }
        else if (v == 1)
        {
                return p;
        }

        if (v % 2 == 1)
        {
                return p + repeatSum(p + p, v / 2);
        }
        else
        {
                return repeatSum(p + p, v / 2);
        }
}

Zp ECsystem::power(Zp val, mpz_class pow) {
        //Find the sum of val*val+...+val (pow times)
        if (pow == 0)
        {
                return 1;
        }
        else if (pow == 1)
        {
                return val;
        }

        if (pow % 2 == 1)
        {
                return val * power(val * val, pow / 2);
        }
        else
        {
                return power(val * val, pow / 2);
        }
}


mpz_class ECsystem::pointCompress(ECpoint e) {
        //It is the gamma function explained in the assignment.
        //Note: Here return type is mpz_class because the function may
        //map to a value greater than the defined PRIME number (i.e, range of Zp)
        //This function is fully defined.
        mpz_class compressedPoint = e.x.getValue();
        compressedPoint = compressedPoint<<1;

        if(e.infinityPoint) {
                cout<<"Point cannot be compressed as its INF-POINT"<<flush;
                abort();
        }
        else {
                if (e.y.getValue()%2 == 1)
                        compressedPoint = compressedPoint + 1;
        }
        //cout<<"For point  "<<e<<"  Compressed point is <<"<<compressedPoint<<"\n";
        return compressedPoint;

}

ECpoint ECsystem::pointDecompress(mpz_class compressedPoint){
        //Implement the delta function for decompressing the compressed point
        mpz_class br = compressedPoint & 1;

        Zp x(compressedPoint >> 1);

        Zp z = Zp(power(x, 3).getValue() + A * x.getValue() + B);

        Zp first_root = power(z, (PRIME + 1) / 4);

        if ((first_root.getValue() & br) == 1)
        {
                return ECpoint(x, first_root);
        }
        else
        {
                Zp second_root(PRIME - first_root.getValue());
                return ECpoint(x, second_root);
        }
}


pair<mpz_class, mpz_class> ECsystem::encrypt(ECpoint publicKey, mpz_class privateKey,mpz_class plaintext){
        // You must implement elliptic curve encryption
        //  Do not generate a random key. Use the private key that is passed from the main function
        ECpoint q = G * privateKey;
        ECpoint r = publicKey * privateKey;
        mpz_class c1 = pointCompress(q);
        mpz_class c2 = plaintext ^ pointCompress(r);
        return make_pair(c1, c2);
}


mpz_class ECsystem::decrypt(pair<mpz_class, mpz_class> ciphertext){
        // Implement EC Decryption
        mpz_class c1 = ciphertext.first;
        mpz_class c2 = ciphertext.second;
        ECpoint r = pointDecompress(c1) * privateKey;
        mpz_class message = c2 ^ pointCompress(r);
        return message;
}


/*
 * main: Compute a pair of public key and private key
 *       Generate plaintext (m1, m2)
 *       Encrypt plaintext using elliptic curve encryption
 *       Decrypt ciphertext using elliptic curve decryption
 *       Should get the original plaintext
 *       Don't change anything in main.  We will use this to 
 *       evaluate the correctness of your program.
 */


int main(void){
        srand(time(0));
        ECsystem ec;
        mpz_class incrementVal;	
        pair <ECpoint, mpz_class> keys = ec.generateKeys();


        mpz_class plaintext = MESSAGE;
        ECpoint publicKey = keys.first;
        cout<<"Public key is: "<<publicKey<<"\n";

        cout<<"Enter offset value for sender's private key"<<endl;
        cin>>incrementVal;
        mpz_class privateKey = XB + incrementVal;

        pair<mpz_class, mpz_class> ciphertext = ec.encrypt(publicKey, privateKey, plaintext);	
        cout<<"Encrypted ciphertext is: ("<<ciphertext.first<<", "<<ciphertext.second<<")\n";
        mpz_class plaintext1 = ec.decrypt(ciphertext);

        cout << "Original plaintext is: " << plaintext << endl;
        cout << "Decrypted plaintext: " << plaintext1 << endl;


        if(plaintext == plaintext1)
                cout << "Correct!" << endl;
        else
                cout << "Plaintext different from original plaintext." << endl;	

        return 1;

}


