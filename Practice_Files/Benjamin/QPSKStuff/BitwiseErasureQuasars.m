close all, clear all;
sigmaSquared = .8;
G = 1;
epsilon = .1;
b = [1 -1];
c = [j -j];

%%%%%%%%%%%%%%%%%%%
% Creates the QPSK symbol locations
a = [1+j -1+j -1-j 1-j];

for re = -50:50         % -50:50 creates a 101 x 101 grid
    for im = -50:50
        x(re+51,im+51) = (re/25) + (im/25)*j;
    end
end
real_x = real(x);
imag_x = imag(x);

fx_real = zeros(2, 101, 101);
fxSum_real = zeros(1, 101, 101);
for loop = 1:2
    fx_real(loop,:,:) = (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((x-1*b(loop)).^2));
    fxSum_real(1, :,:) = fxSum_real(1, :,:) + fx_real(loop,:,:);
end
fxSum_real = fxSum_real .* .5;

    fx_imag = zeros(2, 101, 101);
    fxSum_imag = zeros(1, 101, 101);
    for loop = 1:2
        fx_imag(loop,:,:) = (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((x-1*c(loop)).^2));
        fxSum_imag(1, :,:) = fxSum_imag(1, :,:) + fx_imag(loop,:,:);
    end
    fxSum_imag = fxSum_imag .* .5;

p_b= zeros(2, 101, 101);
for n = 1:2
    p_b(n,:,:) = fx_real(n,:,:) ./ (2*fxSum_real(1,:,:));
end

    p_c= zeros(2, 101, 101);
    for n = 1:2
        p_c(n,:,:) = fx_imag(n,:,:) ./ (2*fxSum_imag(1,:,:));
    end

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
    
H = H_real + H_imag;

    
I_real = 1 - H_real;
    I_imag = 1 - H_imag;
    
    I = I_real + I_imag;

bits_real = zeros(101,101);
for c = 1:101
    for d = 1:101
        if (I_real(c,d) < epsilon)
            bits_real(c,d) = 1;
        else
            bits_real (c,d) = 0;
        end
    end
end


    bits_imag = zeros(101,101);
    for c = 1:101
        for d = 1:101
            if (I_imag(c,d) < epsilon)
                bits_imag(c,d) = 1;
            else
                bits_imag (c,d) = 0;
            end
        end
    end

bits = bits_imag + bits_real;

%% Graphs

%%%%% Plot Equivocation
figure();
hold on
plot(a,'o', 'MarkerFaceColor', 'white', 'Color', 'black'); % Need to fix where this plots on the z axis
% surface(real_x, imag_x, H_real);
% surface(real_x, imag_x, H_imag);
surface(real_x, imag_x, H);
title("\sigma^2 = " + string(sigmaSquared));
xlabel('real');
ylabel('imaginary');
zlabel('Equivocation');
view(45, 60);
colormap(jet);
hold off

%%%%% Plot Mutual Information
figure();
hold on
plot(a,'o', 'MarkerFaceColor', 'white', 'Color', 'black'); % Need to fix where this plots on the z axis
% surface(real_x, imag_x, I_real);
% surface(real_x, imag_x, I_imag);
surface(real_x, imag_x, I);
title("\sigma^2 = " + string(sigmaSquared));
xlabel('real');
ylabel('imaginary');
zlabel('Mutual Information');
view(45, 60);
colormap(jet);
hold off


%%%%% Bits lost
figure();
hold on
plot(a,'o', 'MarkerFaceColor', 'white', 'Color', 'black'); % Need to fix where this plots on the z axis
% surface(real_x, imag_x, bits_real);
% surface(real_x, imag_x, bits_imag);
surface(real_x, imag_x, bits);
title("\sigma^2 = " + string(sigmaSquared));
xlabel('real');
ylabel('imaginary');
zlabel('Bits');
view(45, 60);
colormap(jet);
