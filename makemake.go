package main

import (
	"io/fs"
	"os"
	"path/filepath"
	"slices"
	"strings"
)

var (
	strEnds   = strings.HasSuffix
	strBegins = strings.HasPrefix
	strLibs   = map[string][]string{
		"mo2d": {"sfml-graphics", "sfml-window", "sfml-system", "GL"},
	}
)

type makeRule struct {
	outPath  string
	inPath   string
	depPaths []string
}

func main() {
	executable_names := os.Args[1:]
	var lib_dep_names []string
	cur_dir_path := fsAbs(".")
	var all_source_file_paths []string
	fs.WalkDir(os.DirFS(cur_dir_path), ".", func(fsPath string, fsEntry fs.DirEntry, err error) error {
		if err != nil {
			panic(err)
		}
		if fsEntry.IsDir() && strBegins(fsPath, "libdeps/") && (1 == strings.Count(fsPath, "/")) {
			lib_dep_names = append(lib_dep_names, filepath.Base(fsPath))
		}
		if !strBegins(fsPath, "mo") {
			return nil
		}
		if (!fsEntry.IsDir()) && (strEnds(fsPath, ".cpp") || strEnds(fsPath, ".h")) {
			all_source_file_paths = append(all_source_file_paths, fsPath)
		}
		return nil
	})

	var rules []makeRule
	for _, source_file_path := range all_source_file_paths {
		if strEnds(source_file_path, ".cpp") {
			rule := makeRule{inPath: source_file_path, outPath: fsCppPathToObjPath(source_file_path)}
			src := fsRead(rule.inPath)
			for again, needle_incl := true, "#include \"."; again; {
				idx_incl := strings.LastIndex(src, needle_incl)
				if again = (idx_incl >= 0); again {
					idx_off := idx_incl + len(needle_incl) - len(".")
					idx_quot := strings.IndexByte(src[idx_off:], '"')
					if again, idx_quot = (idx_quot > 0), idx_off+idx_quot; again {
						h_file_path := filepath.Join(filepath.Dir(rule.inPath), src[idx_off:idx_quot])
						cpp_file_path := h_file_path[:len(h_file_path)-len(".h")] + ".cpp"
						for _, src_file_path := range []string{h_file_path, cpp_file_path} {
							if fsIsFile(src_file_path) && slices.Index(rule.depPaths, src_file_path) < 0 {
								rule.depPaths = append(rule.depPaths, src_file_path)
								if strEnds(src_file_path, ".cpp") {
									obj_file_path := fsCppPathToObjPath(src_file_path)
									rule.depPaths = append(rule.depPaths, obj_file_path)
								}
							}
						}
						src = src[:idx_incl]
					}
				}
			}
			rules = append(rules, rule)
		}
	}

	var buf strings.Builder
	for _, rule := range rules { //bin/mo2d_main.o
		for _, executable_name := range executable_names {
			if rule.outPath == "bin/"+executable_name+"_main.o" {
				buf.WriteString(executable_name + ": " + rule.outPath + "\n\t" + rule.outPath + "\n")
				buf.WriteString("\t$(CXX) $(CXXFLAGS) -Lbin")
				buf.WriteString("\n")

				buf.WriteString("\n")
				break
			}
		}
	}
	for _, rule := range rules {
		buf.WriteString(rule.outPath + ": " + rule.inPath)
		for _, dep := range rule.depPaths {
			buf.WriteString(", " + dep)
		}
		buf.WriteString("\n\t$(CXX) -c $(CXXFLAGS) -Ilibdeps/" + strings.Join(lib_dep_names, " -Ilibdeps/") + " " + rule.inPath + " -o " + rule.outPath + "\n\n")
	}
	fsWrite("makemake.nu", buf.String())
}

func iIf[T any](b bool, t T, f T) T {
	if b {
		return t
	}
	return f
}

func fsCppPathToObjPath(fsPath string) string {
	return "bin/" + fsCppPathToObjName(fsPath) + ".o"
}

func fsCppPathToObjName(fsPath string) string {
	idx := strings.LastIndexByte(fsPath, '.')
	return strings.ReplaceAll(fsPath[:idx], "/", "_")
}

func fsIsFile(fsPath string) bool {
	stat, err := os.Stat(fsPath)
	return (err == nil) && (stat != nil) && !stat.IsDir()
}

func fsRead(fsPath string) string {
	raw, err := os.ReadFile(fsPath)
	if err != nil {
		panic(err)
	}
	return string(raw)
}

func fsWrite(fsPath string, src string) {
	err := os.WriteFile(fsPath, []byte(src), os.ModePerm)
	if err != nil {
		panic(err)
	}
}

func fsAbs(fsPath string) string {
	ret, err := filepath.Abs(fsPath)
	if err != nil {
		panic(err)
	}
	return ret
}
