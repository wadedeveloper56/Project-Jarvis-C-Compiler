/////////////////////////////////////////////////
// Files.h - Declaration of the CViewFiles, CContainFiles
//           and CDockFiles classes

#ifndef FILES_H
#define FILES_H


//////////////////////////////////////////////////////
// CViewTree manages a tre view control. It is used as
// the view window for CMDIChildTree.
class CViewTree : public CTreeView
{
public:
    CViewTree() = default;
    virtual ~CViewTree() override;

protected:
    virtual void OnAttach() override;
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) override;

private:
    CViewTree(const CViewTree&) = delete;
    CViewTree& operator=(const CViewTree&) = delete;
};

////////////////////////////////////////////////////
// CContainFiles manages a dock container. It has a
// CViewFiles view.
class CContainFiles : public CDockContainer
{
public:
    CContainFiles();
    virtual ~CContainFiles() override = default;

protected:
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) override;

private:
    CContainFiles(const CContainFiles&) = delete;
    CContainFiles& operator = (const CContainFiles&) = delete;

    CViewTree m_viewTree;
};


/////////////////////////////////////////
// CDockFiles manages a docker that has a
// CContainFiles view.
class CDockFiles : public CDocker
{
public:
    CDockFiles();
    virtual ~CDockFiles() override = default;

protected:
    virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) override;

private:
    CDockFiles(const CDockFiles&) = delete;
    CDockFiles& operator = (const CDockFiles&) = delete;

    CContainFiles m_files;
};


#endif // FILES_H
