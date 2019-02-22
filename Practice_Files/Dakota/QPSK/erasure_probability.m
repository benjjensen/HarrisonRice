function[p_e] = erasure_probability(sigmaSquared,G,epsilon)

a = [1+j -1+j -1-j 1-j];
tic;
%% Functions

fun_x = @(x,y) ((1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((complex(x,y)-(G*a(1))).^2))+...
            (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((complex(x,y)-(G*a(2))).^2))+...
            (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((complex(x,y)-(G*a(3))).^2))+...
            (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((complex(x,y)-(G*a(4))).^2)))...
            .*.25;

p_a = @(x,y,n) (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((complex(x,y)-(G*a(n))).^2))...
    /(4*fun_x(x,y));

H = @(x,y) -p_a(x,y,1).*log2(p_a(x,y,1)) - p_a(x,y,2).*log2(p_a(x,y,2))...
    - p_a(x,y,3).*log2(p_a(x,y,3)) - p_a(x,y,4).*log2(p_a(x,y,4));

I = @(x,y) 2-H(x,y);

e_2 = @(x,y) (I(x,y) < epsilon);
e_3 = @(x,y) (I(x,y) < 1+epsilon);

ymin1 = @(x) fzz2(e_3,x,G);
ymax1 = @(x) fzz(e_3,x,G);
ymin2 = @(x) fzz2(e_2,x,G);
ymax2 = @(x) fzz(e_2,x,G);


p1 = integral2(fun_x,-inf,inf,ymin1,ymax1);
p2 = integral2(fun_x,-inf,inf,ymin2,ymax2);

toc;
p_e = (p1-p2)*.5 + p2;


