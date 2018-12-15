close all; clear;

a = [1+j -1+j -1-j 1-j];

epsilon = .01;
sigmaSquared = .2;

    for re = -50:50         % -50:50 creates a 101 x 101 grid 
        for im = -50:50
            x(re+51,im+51) = (re/25) + (im/25)*j;
        end
    end
    real_x = real(x);
    imag_x = imag(x);

fx_a = zeros(4, 101, 101);
Sum_fx_a = zeros(1,101, 101);
for loop = 1:4
    fx_a(loop,:,:) = (1/(2*pi*sigmaSquared))*exp((-1/(2*sigmaSquared))*abs((x-a(loop)).^2));
    Sum_fx_a(1,:,:) = Sum_fx_a(1,:,:) + fx_a(loop,:,:);
end
fx = squeeze(Sum_fx_a) * .25;

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
    fun = @(P1,x) 
    One = bits == 1;
    Two = bits == 2;
    Two = Two/2;
    P1 = fx .* One;
    P2 = fx .* Two;
    PP = integral2(P1,real_x(1),real_x(101),imag_x(1),imag_x(101));
    