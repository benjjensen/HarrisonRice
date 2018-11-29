%%%%% tempLiklihood
close all;
clear all;

a = [1+j -1+j -1-j 1-j];
%sigmaSquared = .3;

for sigmaSquared = .1:.1:1.1
    for re = -50:50
        for im = -50:50
            x(re+51,im+51) = (re/25) + (im/25)*j;
        end
    end
    real_x = real(x);
    imag_x = imag(x);

    fx = zeros(4, 101, 101);
    fxSum = zeros(1, 101, 101);
    for loop = 1:4
        fx(loop,:,:) = (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((x-a(loop)).^2));
        fxSum(1, :,:) = fxSum(1, :,:) + fx(loop,:,:);
    end

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

    figure();
    hold on
    plot(a,'o'); % Need to fix where this plots on the z axis
    surface(real_x, imag_x, H);
    title("\sigma^2 = " + string(sigmaSquared));
    xlabel('real');
    ylabel('imaginary');
    zlabel('Equivocation');
    view(45, 60);
    colormap(jet);
    hold off
end