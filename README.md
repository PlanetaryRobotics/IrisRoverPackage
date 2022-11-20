# IrisRoverPackage
Complete software package for the Iris Lunar Rover (CMU).

**NOTE**: This repository used to be called `CubeRoverPackage`. For legacy reasons, the URL `https://github.com/PlanetaryRobotics/CubeRoverPackage` still works along with all its subpages. The name `CubeRoverPackage` predates the split of CubeRover and Iris Lunar Rover. This codebase is entirely unaffiliated with `CUBEROVER™`, a trademark of Astrobotic Technology.

## Development
### Highlevel Late-Stage Branching Philosophy:
The following is the policy for branching as of mid-2022 in the lead up to mission as final FSW release candidates are prepared.

#### General Branching Structure:
```
                           master
                             |                                     
                             |                                  |-
                       master-staging - - - - - - - - - - - - - |-  ...  [GSW feature branches]
                             |                                  |-
                             |
                     release-candidate
                         |   |   |
                   [RC feature branches]
```

#### Branch Definitions:
- `master`: Complete Iris Lunar Rover Software Package.
- `master-staging`: Draft code for `master`. Only contains **released** FSW (FSW that's on the Flight Rover).
    - Used for performing integrated tests of the full software stack.
- `release-candidate`: Draft FSW + GSW used for a Release Candidate (RC) in preparation for a **Flight Release** (uploading code to the Flight Rover).
    - Every time code is uploaded to the flight rover, that constitutes a new **Flight Release**, at which point `release-candidate` is PR'd into `master-staging`.
    - Release Candidates that became Flight Releases can be found using `tags` (e.g. [`RC5`](https://github.com/PlanetaryRobotics/IrisRoverPackage/releases/tag/rc5), [`RC6`](https://github.com/PlanetaryRobotics/IrisRoverPackage/releases/tag/rc6), etc.). For tracking purposes, these preserve their "`RC`" names.
- `[RC feature branches]`: Feature / bug-fix branches containing FSW changes required for a new **Flight Release**.
    - Only get pushed to `release-candidate` when the features are ready for a Flight Release (i.e. done and tested).
    - Can also contain GSW changes if *directly related* to FSW changes (e.g. a new serialized type is added FSW that requires new serialization code in GSW).
- `[GSW feature branches]`: GSW-only feature / bug-fix branches that are independent of FSW feature changes / need access to the **latest Flight Release**.
    - This is important because some GSW features compile FSW for message definitions. For the sake of accurate testing, it's imperative that the FSW used by this GSW reflects the latest state of the **released** FSW.
