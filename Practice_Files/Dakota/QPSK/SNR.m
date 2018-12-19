close all; clear;

a = [1+j -1+j -1-j 1-j];

epsilon = .01;
sigmaSquared = .4;

for re = -50:50         % -50:50 creates a 101 x 101 grid
    for im = -50:50
        x(re+51,im+51) = (re/25) + (im/25)*1i;
    end
end
real_x = real(x);
imag_x = imag(x);

[fx,fx_a, Sum_fx_a] = fz(1,sigmaSquared,x);

fun = @fzz;

% q = integral2(fun,51,101,51,101);

figure();
hold on
%     plot(a,'o', 'MarkerFaceColor', 'white', 'Color', 'black'); % Need to fix where this plots on the z axis
surface(real_x, imag_x, fx);
title("\sigma^2 = " + string(sigmaSquared));
xlabel('real');
ylabel('imaginary');
zlabel('Pr(x)');
view(45, 60);
colormap(jet);
hold off

% %     I have f(x) now find mutual info reg. where 1? and 2? and integrate
% over f(x) over those regions

p_a = zeros(4, 101, 101);
for loop = 1:4
    p_a(loop,:,:) = fx_a(loop,:,:) ./ Sum_fx_a;
end

H = zeros(101, 101);
% probOfX = 1 / (101*101);
sumOfProb = zeros(101, 101);
for xCoord = -50:50
    for yCoord = -50:50
        for loop = 1:4
            sumOfProb(xCoord+51,yCoord+51) = sumOfProb(xCoord+51,yCoord+51) + p_a(loop,xCoord+51,yCoord+51)*log2(p_a(loop,xCoord+51,yCoord+51));
        end
        H((xCoord+51), (yCoord+51)) = -(sumOfProb(xCoord+51, yCoord+51));
    end
end

I = 2 - H;

bits = zeros(101,101);

for c = 1:101
    for d = 1:101
        if (I(c,d) < epsilon)
            bits(c,d) = 2;
        elseif (I(c,d) < 1+epsilon)
            bits(c,d) = 1;
        else
            bits (c,d) = 0;
        end
    end
end
One = bits == 1;
Two = bits == 2;
Two = Two/2;
P1 = P_e(fx,One);
P2 = P_e(fx,Two);

figure();
hold on
%     plot(a,'o', 'MarkerFaceColor', 'white', 'Color', 'black'); % Need to fix where this plots on the z axis
surface(real_x, imag_x, P1);
title("\sigma^2 = " + string(sigmaSquared));
xlabel('real');
ylabel('imaginary');
zlabel('Pr(x)');
view(45, 60);
colormap(jet);
hold off

figure();
hold on
%     plot(a,'o', 'MarkerFaceColor', 'white', 'Color', 'black'); % Need to fix where this plots on the z axis
surface(real_x, imag_x, P2);
title("\sigma^2 = " + string(sigmaSquared));
xlabel('real');
ylabel('imaginary');
zlabel('Pr(x)');
view(45, 60);
colormap(jet);
hold off
% f = @P_e;
% PP = integral2(f,real_x(1),real_x(101),imag_x(1),imag_x(101));

C = [0+0i 10+0i 10+10i 0+10i];
G = 1;

% fun = @pdf_z; 
fun = @(x,y) ((1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((complex(x,y)-(G*a(1))).^2))+...
            (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((complex(x,y)-(G*a(2))).^2))+...
            (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((complex(x,y)-(G*a(3))).^2))+...
            (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((complex(x,y)-(G*a(4))).^2)))...
            *.25;
q1 = integral2(fun, 0,inf,0,inf);
q2 = integral2(fun,-inf,0,0,inf);
q3 = integral2(fun,-inf,0,-inf,0);
q4 = integral2(fun,0,inf,-inf,0);


