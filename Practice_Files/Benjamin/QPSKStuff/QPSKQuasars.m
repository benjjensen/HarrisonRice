%%%%% Practice - Quasars from our data
close all; clear all;

%%% Load stuff
% load('C:\Users\benjj\Work\EcEnResearchAssistant\HarrisonRice\Practice_Files\Kalin\QPSK\linear_noisefloor.mat');
% load('C:\Users\benjj\Work\EcEnResearchAssistant\HarrisonRice\Practice_Files\Kalin\QPSK\linear_signal.mat');
load('linear_noisefloor.mat');
load('linear_signal.mat');

% Creates the QPSK symbol locations
a = [1+j -1+j -1-j 1-j];

sigmaSquared = zeros(64, 90, 345);
H2 = zeros(64, 90, 345);

% for heatmapX = 1:345
%     for heatmapY = 1:90
heatmapX = 10;
heatmapY = 10;
carrier = 45;

        %for carrier = 45:45
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
            xlabel('real');
            ylabel('imaginary');
            zlabel('Equivocation');
            view(45, 60);
            colormap(jet);
            hold off ;            
        %end
        
%     end
% end
