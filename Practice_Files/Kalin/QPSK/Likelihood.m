close all;
clear;

plot_qpsk = false;
single_sigma = false;
    sigma_squared = .9;
    plot_individually = false;
    plot_combined = false;
multiple_sigma = true;

a = [1+j -1+j -1-j 1-j];
%% Basic Plot of QPSK
if plot_qpsk
    % a0 = 1+j -> top right         a1  |  a0
    % a1 = -1+j -> top left         ____|____
    % a2 = -1-j -> bottom left          |
    % a3 = 1-j -> bottom right      a2  |  a3
    figure();
    plot(a, 'o');
    xlim([-2 2]);
    ylim([-2 2]);
    %%% Plots axis
    xL = xlim;
    yL = ylim;
    line([0 0], yL, 'Color', 'black');  %x-axis
    line(xL, [0 0], 'Color', 'black');  %y-axis
    %%%
    grid on
end

%% Calculate Likelihood and Probabilities of a_m Given x
if single_sigma
    for re = -50:50
        for im = -50:50
            x(re+51,im+51) = (re/25) + (im/25)*j;
        end
    end
    real_x = real(x);
    imag_x = imag(x);
    
    f_x_a0 = (1/(2*pi*(sigma_squared)))*exp((-1/(2*(sigma_squared)))*abs((x-a(1)).^2));
    f_x_a1 = (1/(2*pi*(sigma_squared)))*exp((-1/(2*(sigma_squared)))*abs((x-a(2)).^2));
    f_x_a2 = (1/(2*pi*(sigma_squared)))*exp((-1/(2*(sigma_squared)))*abs((x-a(3)).^2));
    f_x_a3 = (1/(2*pi*(sigma_squared)))*exp((-1/(2*(sigma_squared)))*abs((x-a(4)).^2));
    f_x_sum = f_x_a0 + f_x_a1 + f_x_a2 + f_x_a3;
    p_a0_x = f_x_a0 ./ f_x_sum;
    p_a1_x = f_x_a1 ./ f_x_sum;
    p_a2_x = f_x_a2 ./ f_x_sum;
    p_a3_x = f_x_a3 ./ f_x_sum;
    
    %% Plot Probabilities Individually
    if plot_individually
        
        figure();
        hold on
        plot(a,'o');
        surface(real_x, imag_x, p_a0_x);
        hold off
        
        figure();
        hold on
        plot(a,'o');
        surface(real_x, imag_x, p_a1_x);
        hold off
        
        figure();
        hold on
        plot(a,'o');
        surface(real_x, imag_x, p_a2_x);
        hold off
        
        figure();
        hold on
        plot(a,'o');
        surface(real_x, imag_x, p_a3_x);
        hold off
    end
    
    %% Plot Probabilites Together
    if plot_combined
        p_am_x = max(p_a0_x, p_a1_x);
        p_am_x = max(p_a2_x, p_am_x);
        p_am_x = max(p_a3_x, p_am_x);
        
        figure();
        hold on
        plot(a,'o'); % Need to fix where this plots on the z axis
        surface(real_x, imag_x, p_am_x);
        hold off
    end
end

%% Multiple Sigma
if multiple_sigma
    
    for re = -50:50
        for im = -50:50
            x(re+51,im+51) = (re/25) + (im/25)*j;
        end
    end
    real_x = real(x);
    imag_x = imag(x);
    
    for loops = 1:10
        sigma_squared = loops/10;
        f_x_a0(loops,:,:) = (1/(2*pi*(sigma_squared)))*exp((-1/(2*(sigma_squared)))*abs((x-a(1)).^2));
        f_x_a1(loops,:,:) = (1/(2*pi*(sigma_squared)))*exp((-1/(2*(sigma_squared)))*abs((x-a(2)).^2));
        f_x_a2(loops,:,:) = (1/(2*pi*(sigma_squared)))*exp((-1/(2*(sigma_squared)))*abs((x-a(3)).^2));
        f_x_a3(loops,:,:) = (1/(2*pi*(sigma_squared)))*exp((-1/(2*(sigma_squared)))*abs((x-a(4)).^2));
        f_x_sum(loops,:,:) = f_x_a0(loops,:,:) + f_x_a1(loops,:,:) + f_x_a2(loops,:,:) + f_x_a3(loops,:,:);
        p_a0_x(loops,:,:) = f_x_a0(loops,:,:) ./ f_x_sum(loops,:,:);
        p_a1_x(loops,:,:) = f_x_a1(loops,:,:) ./ f_x_sum(loops,:,:);
        p_a2_x(loops,:,:) = f_x_a2(loops,:,:) ./ f_x_sum(loops,:,:);
        p_a3_x(loops,:,:) = f_x_a3(loops,:,:) ./ f_x_sum(loops,:,:);
        
        p_am_x(loops,:,:) = max(p_a0_x(loops,:,:), p_a1_x(loops,:,:));
        p_am_x(loops,:,:) = max(p_a2_x(loops,:,:), p_am_x(loops,:,:));
        p_am_x(loops,:,:) = max(p_a3_x(loops,:,:), p_am_x(loops,:,:));
        
        probability_plot(:,:) = p_am_x(loops,:,:);
        probability_at_a = [probability_plot(76,76) probability_plot(26,76) probability_plot(26,76) probability_plot(76,26)];
        figure();
        hold on
        plot3(real(a), imag(a), probability_at_a, 'o', 'MarkerFaceColor', 'black', 'Color', 'black');
        surface(real_x, imag_x, probability_plot);
        title("\sigma^2 = " + string(sigma_squared));
        xlabel('real');
        ylabel('imaginary');
        zlabel('symbol probability');
        view(45, 60);
        colormap(jet);
        hold off
    end
end