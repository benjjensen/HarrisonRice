% Determines the equivocation and mutual information on a transmission as a
% function of location

%   Works bit-by-bit rather than symbol-by-symbol
%   Treats each axis separately, but then they can be combined together
%   Results in cross-like shapes

% Process:
%    Generates four QPSK locations, then calculates the likelihood, then
%    probability, then equivocation, and finally the mutual information in
%    that order. Works bit-by-bit 

% See GUI_Bitwise.mlapp for examples

%% Set Up
close all;
clear;

sigmaSquared = .5;  % Sigma Squared ( = No/2)
G = 1;              % Channel Gain (H in the likelihood equation, but we are using H for equivocation)
epsilon = .5;       % Threshold used to determine whether or not a bit was erased
b = [1 -1];         % Real bit
c = [j -j];         % Imaginary bit

% Creates the QPSK symbol locations
a = [1+j -1+j -1-j 1-j];

for re = -50:50         % -50:50 creates a 101 x 101 grid
    for im = -50:50
        x(re+51,im+51) = (re/25) + (im/25)*j;
    end
end
real_x = real(x);
imag_x = imag(x);


    % Likelihood function for the real bit
fx_real = zeros(2, 101, 101);
fxSum_real = zeros(1, 101, 101);
for loop = 1:2
    fx_real(loop,:,:) = (1/(sqrt(2*pi*sigmaSquared)))*exp((-1/(2*sigmaSquared))*abs((x-G*b(loop)).^2));
    fxSum_real(1, :,:) = fxSum_real(1, :,:) + fx_real(loop,:,:);
end
fxSum_real = fxSum_real .* .5;

    % Likelihood function for the imaginary bit
fx_imag = zeros(2, 101, 101);
fxSum_imag = zeros(1, 101, 101);
for loop = 1:2
    fx_imag(loop,:,:) = (1/(sqrt(2*pi*sigmaSquared)))*exp((-1/(2*sigmaSquared))*abs((x-G*c(loop)).^2));
    fxSum_imag(1, :,:) = fxSum_imag(1, :,:) + fx_imag(loop,:,:);
end
fxSum_imag = fxSum_imag .* .5;


   % Probability for real bit 
p_b= zeros(2, 101, 101);
for n = 1:2
    p_b(n,:,:) = fx_real(n,:,:) ./ (2*fxSum_real(1,:,:));
end

    % Probability for imaginary bit
p_c= zeros(2, 101, 101);
for n = 1:2
    p_c(n,:,:) = fx_imag(n,:,:) ./ (2*fxSum_imag(1,:,:));
end


    % Equivocation for real part
H_real = zeros(101, 101);
sumOfProb_real = zeros(101, 101);
for xCoord = -50:50
    for yCoord = -50:50
        for loop = 1:2
            sumOfProb_real(xCoord+51,yCoord+51) = sumOfProb_real(xCoord+51,yCoord+51) + ...
                p_b(loop,xCoord+51,yCoord+51)*log2(p_b(loop,xCoord+51,yCoord+51));
        end
        H_real((xCoord+51), (yCoord+51)) = -(sumOfProb_real(xCoord+51, yCoord+51));
    end
end

    % Equivocation for imaginary part
H_imag = zeros(101, 101);
sumOfProb_imag = zeros(101, 101);
for xCoord = -50:50
    for yCoord = -50:50
        for loop = 1:2
            sumOfProb_imag(xCoord+51,yCoord+51) = sumOfProb_imag(xCoord+51,yCoord+51) + ...
                p_c(loop,xCoord+51,yCoord+51)*log2(p_c(loop,xCoord+51,yCoord+51));
        end
        H_imag((xCoord+51), (yCoord+51)) = -(sumOfProb_imag(xCoord+51, yCoord+51));
    end
end

   % Combined Equivocation
H = H_real + H_imag;


%% Plot Equivocation
figure();
hold on
plot(a,'o', 'MarkerFaceColor', 'white', 'Color', 'black'); % Need to fix where this plots on the z axis
% surface(real_x, imag_x, H_real);  % These two are used to leave as bits
% surface(real_x, imag_x, H_imag);
surface(real_x, imag_x, H);         % Used if summing the two together
title("\sigma^2 = " + string(sigmaSquared));
xlabel('In-Phase');
ylabel('Quadrature');
zlabel('H(X|z)');
view(45, 60);
colormap(jet);
hold off


%% Mutual Information 

I_real = 1 - H_real;
I_imag = 1 - H_imag;
I = I_real + I_imag;

% Assigns colors to show which areas would result in one erasure, two, etc.
C1(I_real<epsilon) = 0;
C1((I_real>=epsilon)) = 1;
C1 = reshape(C1,size(I_real));

C2(I_imag<epsilon) = 0;
C2((I_imag>=epsilon)) = 1;
C2 = reshape(C2,size(I_imag));

C = C1 + C2;

        % Raises QPSK points to the surface 
a_heights = [(I(76,76)+.01) (I(26,76)+.01) (I(26,26)+.01) (I(76,26)+.01)];

%%%%% Plot Mutual Information
figure();
hold on
plot3(real(a), imag(a), a_heights, 'o', 'MarkerFaceColor', 'white', 'Color', 'black');
%surface(real_x, imag_x, I_real,C1);
%surface(real_x, imag_x, I_imag,C2);
surface(real_x, imag_x, I,C);
xlabel('In-Phase');
ylabel('Quadrature');
zlabel('I(X^n;Z^n)');
view(45, 60);
colormap([0,1,0;1,0,0;0,0,1]);
% saveas(gcf,'BitInformationGraph','epsc');
hold off

%% Mutual Information - Contours
IMax = max(I_imag, I_real);

IMin = min(I_imag, I_real);

figure()
hold on 
contourf(IMin)
contour(IMax, '--')
title("Overlayed Max/Min Graph");
hold off


%% Bits Lost

bits_real = zeros(101,101);
for c = 1:101
    for d = 1:101
        if (I_real(c,d) < epsilon)  % Compares to a threshold 
            bits_real(c,d) = 1;
        else
            bits_real (c,d) = 0;
        end
    end
end


bits_imag = zeros(101,101);
for c = 1:101
    for d = 1:101
        if (I_imag(c,d) < epsilon) % Compares to a threshold
            bits_imag(c,d) = 1;
        else
            bits_imag (c,d) = 0;
        end
    end
end

bits = bits_imag + bits_real;

figure();
hold on
plot(a,'o', 'MarkerFaceColor', 'white', 'Color', 'black'); % Need to fix where this plots on the z axis
% surface(real_x, imag_x, bits_real);
% surface(real_x, imag_x, bits_imag);
surface(real_x, imag_x, bits);
title("\sigma^2 = " + string(sigmaSquared));
xlabel('In-Phase');
ylabel('Quadrature');
zlabel('Bits');
view(45, 60);
colormap(jet);
