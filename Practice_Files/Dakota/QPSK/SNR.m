close all; clear;

a = [1+j -1+j -1-j 1-j];

epsilon = .01;
sigmaSquared = .3;


G = 1; % This is the channel gain (H), but G is used because H is taken for equivocation

%% Functions

fun_x = @(x,y) ((1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((complex(x,y)-(G*a(1))).^2))+...
            (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((complex(x,y)-(G*a(2))).^2))+...
            (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((complex(x,y)-(G*a(3))).^2))+...
            (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((complex(x,y)-(G*a(4))).^2)))...
            *.25;


% %     I have f(x) now find mutual info reg. where 1? and 2? and integrate
% over f(x) over those regions

p_a = @(x,y,n) (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((complex(x,y)-(G*a(n))).^2))...
    /(4*fun_x(x,y));

H = @(x,y) -p_a(x,y,1)*log2(p_a(x,y,1)) - p_a(x,y,2)*log2(p_a(x,y,2))...
    - p_a(x,y,3)*log2(p_a(x,y,3)) - p_a(x,y,4)*log2(p_a(x,y,4));

I = @(x,y) 2-H(x,y);

e_0 = @(x,y) (I(x,y) > 1+epsilon);
e_1 = @(x,y) (I(x,y) < 1+epsilon) && (I(x,y) > epsilon);
e_2 = @(x,y) (I(x,y) < epsilon);
e_3 = @(x,y) (I(x,y) < 1+epsilon);

ymin1 = @(x) 0-fzz(e_3,x,G);
ymax1 = @(x) fzz(e_3,x,G);

p_e0 = @(x,y) e_0(x,y) * fun_x(x,y);
p_e1 = @(x,y) e_1(x,y) * fun_x(x,y);
p_e2 = @(x,y) e_2(x,y) * fun_x(x,y);

% p_e = integral2(p_e1,-inf,inf,-inf,inf)*.5 + integral2(p_e2,-inf,inf,-inf,inf);

% p0 = integral2(p_e0,-2,2,-2,2);
p1 = integral2(fun_x,-inf,inf,ymin1,ymax1);
% p2 = integral2(p_ymin(e2,-10,10,-10,10);

%% matrices 


for re = -50:50         % -50:50 creates a 101 x 101 grid
    for im = -50:50
        x(re+51,im+51) = (re/25) + (im/25)*1i;
    end
end
real_x = real(x);
imag_x = imag(x);

[fx,fx_a, Sum_fx_a] = fz(1,sigmaSquared,x);

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

mp_a = zeros(4, 101, 101);
for loop = 1:4
    mp_a(loop,:,:) = fx_a(loop,:,:) ./ Sum_fx_a;
end

II = zeros(101,101);
HH = zeros(101,101);
probOfX = 1 / (101*101);
sumOfProb = zeros(101,101);
for xCoord = -50:50
    for yCoord = -50:50
        II(xCoord+51,yCoord+51) = I(xCoord/25,yCoord/25);
        for loop = 1:4
            sumOfProb(xCoord+51,yCoord+51) = sumOfProb(xCoord+51,yCoord+51) + mp_a(loop,xCoord+51,yCoord+51)*log2(mp_a(loop,xCoord+51,yCoord+51));
        end
        HH((xCoord+51), (yCoord+51)) = -(sumOfProb(xCoord+51, yCoord+51));
    end
end

% I = 2 - H;

bits = zeros(101,101);

for c = 1:101
    for d = 1:101
        if (II(c,d) < epsilon)
            bits(c,d) = 2;
        elseif (II(c,d) < 1+epsilon)
            bits(c,d) = 1;
        else
            bits (c,d) = 0;
        end
    end
end
One = bits == 1;
Two = bits == 2;
Two = Two/2;
P1 = fx.*One;
P2 = fx.*Two;

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

%% Plots to test functions
for xCoord = -50:50
    for yCoord = -50:50
        p_e0test(xCoord+51,yCoord+51) = p_e0(xCoord/25,yCoord/25);
    end
end
figure();
hold on
%     plot(a,'o', 'MarkerFaceColor', 'white', 'Color', 'black'); % Need to fix where this plots on the z axis
surface(real_x, imag_x, p_e0test);
title("Test of p_e1 with \sigma^2 = " + string(sigmaSquared));
xlabel('real');
ylabel('imaginary');
zlabel('Pr(x)');
view(45, 60);
colormap(jet);
hold off
% 
% figure();
% hold on
% %     plot(a,'o', 'MarkerFaceColor', 'white', 'Color', 'black'); % Need to fix where this plots on the z axis
% surface(real_x, imag_x, P1);
% title("Test of P1 with \sigma^2 = " + string(sigmaSquared));
% xlabel('real');
% ylabel('imaginary');
% zlabel('Pr(x)');
% view(45, 60);
% colormap(jet);
% hold off
% 
% figure();
% hold on
% %     plot(a,'o', 'MarkerFaceColor', 'white', 'Color', 'black'); % Need to fix where this plots on the z axis
% surface(real_x, imag_x, fx);
% title("Test of fx with \sigma^2 = " + string(sigmaSquared));
% xlabel('real');
% ylabel('imaginary');
% zlabel('Pr(x)');
% zlim([0 0.06]);
% view(45, 60);
% colormap(jet);
% hold off


