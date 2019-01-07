%%%%% EQUIVOCATION QUASARS %%%%%
close all; clear all;

% Creates the QPSK symbol locations
a = [1+j -1+j -1-j 1-j];

epsilon = .5;
sigmaSquared = .5;
% for sigmaSquared = .9:.1:1
for re = -50:50         % -50:50 creates a 101 x 101 grid
    for im = -50:50
        x(re+51,im+51) = (re/25) + (im/25)*j;
    end
end
real_x = real(x);
imag_x = imag(x);

fx = zeros(4, 101, 101);
fxSum = zeros(1, 101, 101);
for loop = 1:4
    fx(loop,:,:) = (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((x-1*a(loop)).^2));
    fxSum(1, :,:) = fxSum(1, :,:) + fx(loop,:,:);
end
%fxSum = fxSum/4;

p_a = zeros(4, 101, 101);
for loop = 1:4
    p_a(loop,:,:) = fx(loop,:,:) ./ fxSum;
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


%%%%% Plot Equivocation
% figure();
% hold on
% plot(a,'o', 'MarkerFaceColor', 'white', 'Color', 'black'); % Need to fix where this plots on the z axis
% surface(real_x, imag_x, H);
% title("\sigma^2 = " + string(sigmaSquared));
% xlabel('real');
% ylabel('imaginary');
% zlabel('Equivocation');
% view(45, 60);
% colormap(jet);
% hold off

%%%%% Plot Mutual Information

I = 2 - H;
C(I<epsilon) = 1;
C((I>=epsilon) & (I < (1+epsilon))) = 2;
C(I>=(1+epsilon)) = 3;
C = reshape(C,size(I));

figure();
hold on
plot(a,'o', 'MarkerFaceColor', 'white', 'Color', 'black'); % Need to fix where this plots on the z axis
surface(real_x, imag_x, I, C);
title("Symbol Threshold");
xlabel('In-Phase');
ylabel('Quadrature');
zlabel('Mutual Information');
view(45, 60);
colormap([0,1,0;1,0,0;0,0,1]);
saveas(gcf,'SymbolInformationGraph','epsc');
hold off

bits = zeros(101,101);

for c = 1:101
    for d = 1:101
        if (I(c,d) < epsilon)
            bits(c,d) = 2;
        else if (I(c,d) < 1+epsilon)
                bits(c,d) = 1;
            else
                bits (c,d) = 0;
            end
        end
    end
end
%%%%% Bits lost

% figure();
% hold on
% plot(a,'o', 'MarkerFaceColor', 'white', 'Color', 'black'); % Need to fix where this plots on the z axis
% surface(real_x, imag_x, bits);
% title("\sigma^2 = " + string(sigmaSquared));
% xlabel('real');
% ylabel('imaginary');
% zlabel('Bits');
% view(45, 60);
% colormap(jet);
