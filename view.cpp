#include "solvespace.h"

void TextWindow::ShowEditView(void) {
    Printf(true, "%Ft3D VIEW PARAMETERS%E");

    Printf(true,  "%Bd %Ftoverall scale factor%E");
    Printf(false, "%Ba   %# px/%s %Fl%Ll%f[edit]%E",
        SS.GW.scale * SS.MmPerUnit(),
        SS.UnitName(),
        &ScreenChangeViewScale);
    Printf(false, "");

    Printf(false, "%Bd %Ftorigin (maps to center of screen)%E");
    Printf(false, "%Ba   (%s, %s, %s) %Fl%Ll%f[edit]%E",
        SS.MmToString(-SS.GW.offset.x),
        SS.MmToString(-SS.GW.offset.y),
        SS.MmToString(-SS.GW.offset.z),
        &ScreenChangeViewOrigin);
    Printf(false, "");

    Vector n = (SS.GW.projRight).Cross(SS.GW.projUp);
    Printf(false, "%Bd %Ftprojection onto screen%E");
    Printf(false, "%Ba   %Ftright%E (%3, %3, %3) %Fl%Ll%f[edit]%E",
        CO(SS.GW.projRight),
        &ScreenChangeViewProjection);
    Printf(false, "%Ba   %Ftup%E    (%3, %3, %3)", CO(SS.GW.projUp));
    Printf(false, "%Ba   %Ftout%E   (%3, %3, %3)", CO(n));
    Printf(false, "");

    Printf(false, "The perspective may be changed in the");
    Printf(false, "configuration screen.");
}

void TextWindow::ScreenChangeViewScale(int link, DWORD v) {
    char buf[1024];
    sprintf(buf, "%.3f", SS.GW.scale * SS.MmPerUnit());

    SS.TW.edit.meaning = EDIT_VIEW_SCALE;
    ShowTextEditControl(12, 3, buf);
}

void TextWindow::ScreenChangeViewOrigin(int link, DWORD v) {
    char buf[1024];
    sprintf(buf, "%s, %s, %s",
        SS.MmToString(-SS.GW.offset.x),
        SS.MmToString(-SS.GW.offset.y),
        SS.MmToString(-SS.GW.offset.z));

    SS.TW.edit.meaning = EDIT_VIEW_ORIGIN;
    ShowTextEditControl(18, 3, buf);
}

void TextWindow::ScreenChangeViewProjection(int link, DWORD v) {
    char buf[1024];
    sprintf(buf, "%.3f, %.3f, %.3f", CO(SS.GW.projRight));
    SS.TW.edit.meaning = EDIT_VIEW_PROJ_RIGHT;
    ShowTextEditControl(24, 10, buf);
}

bool TextWindow::EditControlDoneForView(char *s) {
    switch(edit.meaning) {
        case EDIT_VIEW_SCALE: {
            Expr *e = Expr::From(s, true);
            if(e) {
                SS.GW.scale = e->Eval() / SS.MmPerUnit();
            }
            break;
        }

        case EDIT_VIEW_ORIGIN: {
            Vector pt;
            if(sscanf(s, "%lf, %lf, %lf", &pt.x, &pt.y, &pt.z) == 3) {
                pt = pt.ScaledBy(SS.MmPerUnit());
                SS.GW.offset = pt.ScaledBy(-1);
            } else {
                Error("Bad format: specify x, y, z");
            }
            break;
        }

        case EDIT_VIEW_PROJ_RIGHT:
        case EDIT_VIEW_PROJ_UP: {
            Vector pt;
            if(sscanf(s, "%lf, %lf, %lf", &pt.x, &pt.y, &pt.z) != 3) {
                Error("Bad format: specify x, y, z");
                break;
            }
            if(edit.meaning == EDIT_VIEW_PROJ_RIGHT) {
                SS.GW.projRight = pt;
                SS.GW.NormalizeProjectionVectors();
                edit.meaning = EDIT_VIEW_PROJ_UP;
                char buf[1024];
                sprintf(buf, "%.3f, %.3f, %.3f", CO(SS.GW.projUp));
                HideTextEditControl();
                ShowTextEditControl(26, 10, buf);
                edit.showAgain = true;
            } else {
                SS.GW.projUp = pt;
                SS.GW.NormalizeProjectionVectors();
            }
            break;
        }

        default:
            return false;
    }
    return true;
}
