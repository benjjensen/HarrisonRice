% Takes the quasar information and applies our data to it
% NOT FINISHED!

%%
close all; clear all;

%%% Load stuff
% load('C:\Users\benjj\Work\EcEnResearchAssistant\HarrisonRice\Practice_Files\Kalin\QPSK\linear_noisefloor.mat');
% load('C:\Users\benjj\Work\EcEnResearchAssistant\HarrisonRice\Practice_Files\Kalin\QPSK\linear_signal.mat');
load('..HarrisonRice\Practice_files\Kalin\QPSK\linear_noisefloor.mat');
load('..\Practice_files\Kalin\QPSK\linear_signal.mat');

x_coord_start = 10;
x_coord_end = 10;
y_coord_start = 10;
y_coord_end = 10;
carrier_start = 11;
carrier_end = 55;

% Creates the QPSK symbol locations
a = [1+j -1+j -1-j 1-j];

sigmaSquared = zeros(64, 90, 345);
H2 = ones(64, 90, 345);

for heatmapX = x_coord_start:x_coord_end
    for heatmapY = y_coord_start:y_coord_end
        for carrier = carrier_start:carrier_end
            sigmaSquared(carrier, heatmapY, heatmapX) = linear_noisefloor(carrier, heatmapY, heatmapX)/2;
            H2(carrier,heatmapY,heatmapX) = sqrt(linear_signal(carrier,heatmapY,heatmapX));
            
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
                fx(loop,:,:) = (1/(2*pi*sigmaSquared(carrier, heatmapY, heatmapX)))*exp((-1/(2*sigmaSquared(carrier, heatmapY, heatmapX)))*abs((x-H2(carrier, heatmapY, heatmapX)*a(loop)).^2));
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
        
               %%%%% Plot Equivocation
            figure();
            hold on
            plot(a,'o', 'MarkerFaceColor', 'white', 'Color', 'black'); % Need to fix where this plots on the z axis
            surface(real_x, imag_x, H);
            title("\sigma^2 = " + string(sigmaSquared(carrier, heatmapY, heatmapX)) ...
                + "    H= " + string(H2(carrier, heatmapY, heatmapX)));
            xlabel('In-Phase');
            ylabel('Quadrature');
            zlabel('H(X|z)');
            view(45, 60);
            colormap(jet);
            hold off ;            
        end       
    end
end
