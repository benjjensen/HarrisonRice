close all, clear all;
sigmaSquared = .8;
G = 1;
epsilon = .5;
b = [1 -1];

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

fx = zeros(2, 101, 101);
fxSum = zeros(1, 101, 101);
for loop = 1:2
    fx(loop,:,:) = (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((x-1*a(loop)).^2));
    fxSum(1, :,:) = fxSum(1, :,:) + fx(loop,:,:);
end
fxSum = fxSum .* .5;

p_b= zeros(2, 101, 101);
for n = 1:2
    p_b(n,:,:) = fx(n,:,:) ./ (2*fxSum(1,:,:));
end

H = zeros(101, 101);
sumOfProb = zeros(101, 101);

for xCoord = -50:50
    for yCoord = -50:50
        for loop = 1:2
            sumOfProb(xCoord+51,yCoord+51) = sumOfProb(xCoord+51,yCoord+51) + ...
                p_b(loop,xCoord+51,yCoord+51)*log2(p_b(loop,xCoord+51,yCoord+51));
        end
        H((xCoord+51), (yCoord+51)) = -(sumOfProb(xCoord+51, yCoord+51));
    end
end

I = 1 - H;

bits = zeros(101,101);

for c = 1:101
    for d = 1:101
        if (I(c,d) < epsilon)
            bits(c,d) = 1;
        else
            bits (c,d) = 0;
        end
    end
end

% [p_e, q] = DeleteMe(sigmaSquared,G,epsilon);



%% Graphs

%%%%% Plot Equivocation
figure();
hold on
plot(a,'o', 'MarkerFaceColor', 'white', 'Color', 'black'); % Need to fix where this plots on the z axis
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
surface(real_x, imag_x, bits);
title("\sigma^2 = " + string(sigmaSquared));
xlabel('real');
ylabel('imaginary');
zlabel('Bits');
view(45, 60);
colormap(jet);
