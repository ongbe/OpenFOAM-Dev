{
    label nAlphaCorr(readLabel(pimple.dict().lookup("nAlphaCorr")));
    label nAlphaSubCycles(readLabel(pimple.dict().lookup("nAlphaSubCycles")));

    word alphaScheme("div(phi,alpha1)");
    word alpharScheme("div(phir,alpha1)");

    surfaceScalarField phic("phic", phi);
    surfaceScalarField phir("phir", phi1 - phi2);

    if (g0.value() > 0.0)
    {
        surfaceScalarField alpha1f(fvc::interpolate(alpha1));
        surfaceScalarField phipp(ppMagf*fvc::snGrad(alpha1)*mesh.magSf());
        phir += phipp;
        phic += fvc::interpolate(alpha1)*phipp;
    }

    for (int acorr=0; acorr<nAlphaCorr; acorr++)
    {
        for
        (
            subCycle<volScalarField> alphaSubCycle(alpha1, nAlphaSubCycles);
            !(++alphaSubCycle).end();
        )
        {
            surfaceScalarField phiAlpha
            (
                fvc::flux
                (
                    phic,
                    alpha1,
                    alphaScheme
                )
              + fvc::flux
                (
                    -fvc::flux(-phir, alpha2, alpharScheme),
                    alpha1,
                    alpharScheme
                )
            );

            MULES::explicitSolve
            (
                alpha1,
                phi,
                phiAlpha,
                (g0.value() > 0 ? alphaMax : 1),
                0
            );
        }

        if (g0.value() > 0)
        {
            ppMagf = rAU1f*fvc::interpolate
            (
                (1.0/(rho1*(alpha1 + scalar(0.0001))))
               *g0*min(exp(preAlphaExp*(alpha1 - alphaMax)), expMax)
            );

            fvScalarMatrix alpha1Eqn
            (
                fvm::ddt(alpha1) - fvc::ddt(alpha1)
              - fvm::laplacian
                (
                    (fvc::interpolate(alpha1) + scalar(0.0001))*ppMagf,
                    alpha1,
                    "laplacian(alpha1PpMag,alpha1)"
                )
            );

            alpha1Eqn.relax();
            alpha1Eqn.solve();

            #include "packingLimiter.H"
        }

        alpha2 = scalar(1) - alpha1;

        Info<< "Dispersed phase volume fraction = "
            << alpha1.weightedAverage(mesh.V()).value()
            << "  Min(alpha1) = " << min(alpha1).value()
            << "  Max(alpha1) = " << max(alpha1).value()
            << endl;
    }
}

rho = alpha1*rho1 + alpha2*rho2;
