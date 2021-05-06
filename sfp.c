#include "sfp.h"
#include <stdlib.h>

typedef union {
    float f;
    unsigned int a;
}Box;

sfp int2sfp(int input){

    //input --> float
	Box b;
    b.f = (float) input;

    int flag = (b.a >> 31) & 1; //sign..
    int exp = 0;
    sfp answer = 0;
    sfp num = 1;

    if(flag == 1)
        answer = answer | (1 << 15); //set sign..

    //caculate E
    int pow2 = 1;
    for(int i = 23; i <= 30; i++){
        int temp = (b.a >> i) & 1;
        if(temp == 1)
            exp += pow2;
        pow2 *= 2;
    }
    int E = exp -127;

    int ans_Exp = E + 15;   
    // set answer
    int k = 10;
    for(int i = 0; i < 4; i++){
        int temp = ans_Exp % 2;
        ans_Exp /= 2;
        if (temp == 1)
            answer = (1 << k) | answer;
        k++;
    }
    if (ans_Exp == 1)
        answer = (1 << k) | answer;

    int j = 0;
    for(int i = 13; i <= 22; i++){
        int temp = (b.a >> i) & 1;
        if(temp == 1)
            answer = (1 << j) | answer;
        j++;
    }

    if(input == 0) // If input is 0
        return 0;
    if (input > 65504) // +infinity
        return 31744;        // 0 11111 0000000000
    if (input < -65504) // -infinity
        return 64512;        // 1 11111 0000000000

    return answer;
}

int sfp2int(sfp input){
    int flag = (input >> 15) & 1;
    int exp = 0;
    float answer = 1;

    int pow2 = 1;
    for(int i = 10; i <= 14; i++){
        int temp = (input >> i) & 1;
        if(temp == 1)
            exp += pow2;
        pow2 *=2;
    } 
    int E = exp - 15;


    float pow_2 = 1;
    for(int i = 9; i >= 0; i--){
        int temp = (input >> i) & 1;
        pow_2 *= 0.5;
        if(temp == 1)
            answer += pow_2;
    }

    //Denormalized value (0일때)
    if((E == -15) && (answer == 1))
        return 0;


    if(E > 0){
        for(int i = 1; i<= E; i++){
            answer *= 2;
        }
    }
    if(E < 0){
        for(int i = 1; i<= E*(-1); i++){
            answer /= 2;
        }
    }

    if(flag == 1)
        answer *= -1;

    int a = (int) answer;

    if(input == 0)
        return 0;
    //infinity
    if(input == 31744)
        return 2147483647;
    if(input == 64512)
        return -2147483648;
    //NaN
    if((input > 31744 && input < 32768) || (input > 64512 && input < 65535))
        return -2147483648;

    return answer;
}

sfp float2sfp(float input){
    Box b;
    b.f = input;

    int flag = (b.a >> 31) & 1;
    int exp = 0;
    sfp answer = 0;
    sfp num = 1;

    if(flag == 1)
        answer = answer | (1 << 15);

    int pow2 = 1;
    for(int i = 23; i <= 30; i++){
        int temp = (b.a >> i) & 1;
        if(temp == 1)
            exp += pow2;
        pow2 *= 2;
    }
    int E = exp -127;

    int ans_Exp = E + 15;
    
    int k = 10;
    for(int i = 0; i < 4; i++){
        int temp = ans_Exp % 2;
        ans_Exp /= 2;
        if (temp == 1)
            answer = (1 << k) | answer;
        k++;
    }
    if (ans_Exp == 1)
        answer = (1 << k) | answer;

    int j = 0;
    for(int i = 13; i <= 22; i++){
        int temp = (b.a >> i) & 1;
        if(temp == 1)
            answer = (1 << j) | answer;
        j++;
    }

    if(input == 0) // If input is 0
        return 0;
    if (input > 65504) // +infinity
        return 31744;  // 0 11111 0000000000
    if (input < -65504) // -infinity
        return 64512;   // 1 11111 0000000000

    return answer;
}

float sfp2float(sfp input){
    int flag = (input >> 15) & 1;
    int exp = 0;
    float answer = 1;

    int pow2 = 1;
    for(int i = 10; i <= 14; i++){
        int temp = (input >> i) & 1;
        if(temp == 1)
            exp += pow2;
        pow2 *=2;
    } 
    int E = exp - 15;


    float pow_2 = 1;
    for(int i = 9; i >= 0; i--){
        int temp = (input >> i) & 1;
        pow_2 *= 0.5;
        if(temp == 1)
            answer += pow_2;
    }

    //Denormalized value (0일때)
    if((E == -15) && (answer == 1))
        return 0;

    if(E > 0){
        for(int i = 1; i<= E; i++){
            answer *= 2;
        }
    }
    if(E < 0){
        for(int i = 1; i<= E*(-1); i++){
            answer /= 2;
        }
    }

    //infinity
    if(E == 16){
        answer = 0;
        for(int i = 0; i < 128; i++){
            answer *= 2;
        }
    }

    if(flag == 1)
        answer *= -1;

    //NaN
    if((input > 31744 && input < 32768) || (input > 64512 && input < 65535)){
        answer = 1;
        for(int i = 0; i < 128; i++){
            answer *= 2;
        }
        answer = answer -answer;
        return answer;
    }

    return answer;    
}

sfp sfp_add(sfp a, sfp b){
    if((a == 31744) && (b == 31744))
        return 31744;
    if((a == 31744) && (b == 64512))
        return 65535;
    if((a == 64512) && (b == 31744))
        return 65535;
    if((a == 64512) && (b == 64512))
        return 64512;
    if((a > 31744 && a < 32768) || (b > 64512 && b < 65535))
        return 65535;
    if((a == 31744) || (a == 64512))
        return a;
    if((b == 31744) || (b == 64512))
        return b;


    sfp exp_flag = ((1 << 5) - 1) << 10;
    int frac_flag = (1 << 10) - 1;
    sfp hidden_bit = (1 << 10);
    
    sfp res_exp = 0;
    int res_frac = 0;
    sfp result = 0;

    int a_sign = (a >> 15) & 1;
    int b_sign = (b >> 15) & 1;

    sfp a_exp = (a & exp_flag) >> 10;
    sfp b_exp = (b & exp_flag) >> 10;

    int a_frac = (a & frac_flag) | hidden_bit;
    int b_frac = (b & frac_flag) | hidden_bit;

    int round = 0; //1이면 올림 0이면 버림
    int rest = 0;

    //sign bits are different
    if((a_sign == 1) && (b_sign == 0)){
        int temp_sign = 0;
        
        if(a_exp >= b_exp){
            int exp_diff = a_exp - b_exp;
            //
            for(int i = 0; i < exp_diff; i++){
                if(exp_diff == 0){
                    rest = b_frac & 1;
                    break;
                }   
                int bit = (b_frac >> i) & 1;
                rest |= (bit << i);
            }
            //
            b_frac >>= exp_diff;

            // set sign.. calculate fraction
            if(a_frac > b_frac){
                temp_sign = 1;
                res_frac = a_frac - b_frac;
            }
            else
                res_frac = b_frac - a_frac;
                
            //delete carry
            if(((res_frac >> 11) & 1) == 1){
                res_frac &= ~(1 << 11);
            }

            //If hidden bit is 0...
            if(((res_frac >> 10) & 1) == 0){
                int flag = 0;
                for(int i = 9; i>=0; i--){
                    flag++;
                    if(((res_frac >> i) & 1) == 1)
                        break;
                }

                res_frac <<= flag;
                a_exp -= flag;
            }
            res_frac &= ~hidden_bit; //delete hidden bit..
            //
            int a = exp_diff - 1;
            if(rest == (1 << a)){
                if((res_frac & 1) == 1)
                    round = 1;
                if((res_frac & 1) == 0)
                    round = 0;
            }
            if(rest != 0 && (rest < (1 << a)))
                round = 1;
            if(exp_diff == 0){
                if(rest == 1 && (res_frac & 1) == 1)
                    round = 1;
                else
                    round = 0;
            }
            res_frac -= round;
            //
            //set answer..
            result |= res_frac;
            result |= (a_exp << 10);
            if(temp_sign == 1)
                result |= (1 << 15);

            if(a_exp >= 31 && temp_sign == 0)
                return 31744;
            if(a_exp >= 31 && temp_sign == 1)
                return 64512;
        }
        else{
            int exp_diff = b_exp - a_exp;
            //
            for(int i = 0; i < exp_diff; i++){
                if(exp_diff == 0){
                    rest = a_frac & 1;
                    break;
                }   
                int bit = (a_frac >> i) & 1;
                rest |= (bit << i);
            }
            //
            a_frac >>= exp_diff;

            // set sign.. calculate fraction
            if(a_frac > b_frac){
                temp_sign = 1;
                res_frac = a_frac - b_frac;
            }
            else
                res_frac = b_frac - a_frac;

            //delete carry..
            if(((res_frac >> 11) & 1) == 1){
                res_frac &= ~(1 << 11);
            }
            
            //If hidden bit is 0...
            if(((res_frac >> 10) & 1) == 0){
                int flag = 0;
                for(int i = 9; i>=0; i--){
                    flag++;
                    if((res_frac >> i) & 1 == 1)
                        break;
                }
                res_frac <<= flag;
                b_exp -= flag;
            }
            res_frac &= ~hidden_bit; //delte hidden bit
            //
            int a = exp_diff - 1;
            if(rest == (1 << a)){
                if((res_frac & 1) == 1)
                    round = 1;
                if((res_frac & 1) == 0)
                    round = 0;
            }
            if(rest != 0 && (rest < (1 << a)))
                round = 1;
            if(exp_diff == 0){
                if(rest == 1 && (res_frac & 1) == 1)
                    round = 1;
                else
                    round = 0;
            }
            res_frac -= round;
            //
            //set answer
            result |= res_frac;
            result |= (b_exp << 10);
            if(temp_sign == 1)
                result |= (1 << 15);

            if(b_exp >= 31 && temp_sign == 0)
                return 31744;
            if(b_exp >= 31 && temp_sign == 1)
                return 64512;
        }
        return result;
    }
    if((a_sign == 0) && (b_sign == 1)){
        int temp_sign = 0;

        if(a_exp >= b_exp){
            int exp_diff = a_exp - b_exp;
            //
            for(int i = 0; i < exp_diff; i++){
                if(exp_diff == 0){
                    rest = b_frac & 1;
                    break;
                }   
                int bit = (b_frac >> i) & 1;
                rest |= (bit << i);
            }
            //
            b_frac >>= exp_diff;   

            // set sign.. calculate fraction
            if(b_frac > a_frac){
                temp_sign = 1;
                res_frac = b_frac - a_frac;
            }
            else   
                res_frac = a_frac - b_frac;

            //delete carry..
            if(((res_frac >> 11) & 1) == 1){
                res_frac &= ~(1 << 11);
            }

            //If hidden bit is 0...
            if(((res_frac >> 10) & 1) == 0){
                int flag = 0;
                for(int i = 9; i>=0; i--){
                    flag++;
                    if((res_frac >> i) & 1 == 1)
                        break;
                }
                res_frac <<= flag;
                a_exp -= flag;
            }
            res_frac &= ~hidden_bit; //delete hidden bit
            //
            int a = exp_diff - 1;
            if(rest == (1 << a)){
                if((res_frac & 1) == 1)
                    round = 1;
                if((res_frac & 1) == 0)
                    round = 0;
            }
            if(rest != 0 && (rest < (1 << a)))
                round = 1;
            if(exp_diff == 0){
                if(rest == 1 && (res_frac & 1) == 1)
                    round = 1;
                else
                    round = 0;
            }
            res_frac -= round;
            //
            //set answer
            result |= res_frac;
            result |= (a_exp << 10);
            if(temp_sign == 1)
                result |= (1 << 15);

            if(a_exp >= 31 && temp_sign == 0)
                return 31744;
            if(a_exp >= 31 && temp_sign == 1)
                return 64512;
        }
        else{
            int exp_diff = b_exp - a_exp;
            
            //
            for(int i = 0; i < exp_diff; i++){
                if(exp_diff == 0){
                    rest = a_frac & 1;
                    break;
                }   
                int bit = (a_frac >> i) & 1;
                rest |= (bit << i);
            }
            
            a_frac >>= exp_diff;

            // set sign.. calculate fraction
            if(b_frac > a_frac){
                temp_sign = 1;
                res_frac = b_frac - a_frac;
            }
            else   
                res_frac = a_frac - b_frac;

            //delete carry..
            if(((res_frac >> 11) & 1) == 1){
                res_frac &= ~(1 << 11);
            }

            //when hidden bit is 0...
            if(((res_frac >> 10) & 1) == 0){
                int flag = 0;
                for(int i = 9; i>=0; i--){
                    flag++;
                    if((res_frac >> i) & 1 == 1)
                        break;
                }
                res_frac <<= flag;
                b_exp -= flag;
            }
            res_frac &= ~hidden_bit;

            //
            int a = exp_diff - 1;
            if(rest == (1 << a)){
                if((res_frac & 1) == 1)
                    round = 1;
                if((res_frac & 1) == 0)
                    round = 0;
            }
            if(rest != 0 && (rest < (1 << a)))
                round = 1;
            if(exp_diff == 0){
                if(rest == 1 && (res_frac & 1) == 1)
                    round = 1;
                else
                    round = 0;
            }
            res_frac -= round;
            

            //set answer..
            result |= res_frac;
            result |= (b_exp << 10);
            if(temp_sign == 1)
                result |= (1 << 15);

            if(b_exp >= 31 && temp_sign == 0)
                return 31744;
            if(b_exp >= 31 && temp_sign == 1)
                return 64512;
        }
        return result;
    }

    //sign bits are same
    if(a_exp >= b_exp){
        int exp_diff = a_exp - b_exp;

        for(int i = 0; i < exp_diff; i++){
            if(exp_diff == 0){
                rest = b_frac & 1;
                break;
            }
            int bit = (b_frac >> i) & 1;
            rest |= (bit << i);
        }

        b_frac >>= exp_diff;
        res_frac = a_frac + b_frac;

        if(((res_frac >> 11) & 1) == 1){
            a_exp += 1;
            res_frac >>= 1;
        }
        res_frac &= ~hidden_bit;

        int a = exp_diff - 1;
        if(rest == (1 << a)){
            if((res_frac & 1) == 1)
                round = 1;
            if((res_frac & 1) == 0)
                round = 0;
        }
        if(rest > (1 << a))
            round = 1;
        if(exp_diff == 0){
            if(rest == 1 && (res_frac & 1) == 1)
                round = 1;
            else
                round = 0;
        }
        res_frac += round;

        result |= res_frac;
        result |= (a_exp << 10);
        if(a_sign == 1 && b_sign ==1)
            result |= (1 << 15);

        if(a_exp >= 31 && (a_sign == 0 && b_sign == 0))
            return 31744;
        if(a_exp >= 31 && (a_sign == 1 && b_sign ==1))
            return 64512;
    }
    else{
        int exp_diff = b_exp - a_exp;

        for(int i = 0; i < exp_diff; i++){
            if(exp_diff == 0){
                rest = a_frac & 1;
                break;
            }
            int bit = (a_frac >> i) & 1;
            rest |= (bit << i);
        }

        a_frac >>= exp_diff;
        res_frac = a_frac + b_frac;
        if(((res_frac >> 11) & 1) == 1){
            b_exp += 1;
            res_frac >>= 1;
        }
        res_frac &= ~hidden_bit;

        int a = exp_diff - 1;
        if(rest == (1 << a)){
            if((res_frac & 1) == 1)
                round = 1;
            if((res_frac & 1) == 0)
                round = 0;
        }
        if(rest > (1 << a))
            round = 1;
        if(exp_diff == 0){
            if(rest == 1 && (res_frac & 1) == 1)
                round = 1;
            else
                round = 0;
        }
        res_frac += round;

        result |= res_frac;
        result |= (b_exp << 10);
        if(a_sign == 1 && b_sign ==1)
            result |= (1 << 15);

        if(a_exp >= 31 && (a_sign == 0 && b_sign == 0))
            return 31744;
        if(a_exp >= 31 && (a_sign == 1 && b_sign ==1))
            return 64512;
    }

    return result;
}

sfp sfp_mul(sfp a, sfp b){
    // special result
    if((a == 31744) && (b == 31744))
        return 31744;
    if((a == 31744) && (b == 64512))
        return 64512;
    if((a == 64512) && (b == 31744))
        return 64312;
    if((a == 64512) && (b == 64512))
        return 31744;
    if((a > 31744 && a < 32768) || (b > 64512 && b < 65535))
        return 65535;
    if((a == 64512 || a == 31744) && (b == 0))
        return 65535;
    if((b == 64512 || b == 31744) && (a == 0))
        return 65535;
    if((a == 31744) || (a == 64512))
        return a;
    if((b == 31744) || (b == 64512))
        return b;

    if((a == 0) || (b == 0))
        return 0;
    

    sfp exp_flag = ((1 << 5) - 1) << 10;
    int frac_flag = (1 << 10) - 1;
    sfp hidden_bit = (1 << 10);
    
    sfp res_exp = 0;
    int res_frac = 0;
    sfp result = 0;

    int a_sign = (a >> 15) & 1;
    int b_sign = (b >> 15) & 1;

    sfp a_exp = (a & exp_flag) >> 10;
    sfp b_exp = (b & exp_flag) >> 10;
    int a_frac = (a & frac_flag) | hidden_bit;
    int b_frac = (b & frac_flag) | hidden_bit; 

    res_exp = a_exp + b_exp - 15;

    res_frac = a_frac * b_frac;

    if(res_exp >= 31 && (a_sign + b_sign != 1))
        return 31744;
    if(res_exp >= 31 && (a_sign + b_sign == 1))
        return 64512;


    //round to even
    int round = 0; //1이면 올림, 0이면 버림..
    int rest = 0;
    int temp = (res_frac >> 21) & 1;
    if(temp == 1){
        for(int i = 0; i <= 10; i++){
            int bit = (res_frac >> i) & 1;
            rest |= (bit << i);
        }

        res_frac >>= 11;
        res_exp += 1;

        if(rest == (1 << 10)){
            if((res_frac & 1) == 1)
                round = 1;
            if((res_frac & 1) == 0)
                round = 0;
        }
        if(rest > (1 << 10))
            round = 1;
        res_frac += round;
    }
    else{
        for(int i = 0; i <= 9; i++){
            int bit = (res_frac >> i) & 1;
            rest |= (bit << i);
        }

        res_frac >>= 10;

        if(rest == (1 << 9)){
            if((res_frac & 1) == 1)
                round = 1;
            if((res_frac & 1) == 0)
                round = 0;
        }
        if(rest > (1 << 9))
            round = 1;
        res_frac += round;
    }
    res_frac &= ~hidden_bit;

    sfp answer = (res_exp << 10) | res_frac;

    if(a_sign + b_sign == 1)
        answer = answer | (1 << 15);
    
    return answer;
}

char* sfp2bits(sfp result){
    char* ans = (char*)malloc(sizeof(char)*17);
    int shiftNum = 1;
    int flag = result & 1;
    if(flag == 1)
    {
        ans[15] = '1';
    }
    else
    {
        ans[15] = '0';
    }

    for(int i = 14; i>=0; i--){
        flag = (result >> shiftNum) & 1;
        if(flag == 1){
            ans[i] = '1';
        }
        else{
            ans[i] = '0';
        }
        shiftNum++;
    }

    return ans;
}
